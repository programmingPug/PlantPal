namespace house_plant_api.Services
{
    using System;
    using System.Linq;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Extensions.Hosting;
    using Microsoft.Extensions.Logging;
    using Microsoft.EntityFrameworkCore;
    using InTheHand.Bluetooth;
    using house_plant_api.Context;
    using house_plant_api.Models;

    public class DevicePollingService : BackgroundService
    {
        private readonly ILogger<DevicePollingService> _logger;
        private readonly BluetoothService _bluetoothService;
        private readonly IDbContextFactory<AppDbContext> _dbContextFactory;
        private readonly TimeSpan _pollingInterval = TimeSpan.FromSeconds(10);

        public DevicePollingService(
            ILogger<DevicePollingService> logger,
            BluetoothService bluetoothService,
            IDbContextFactory<AppDbContext> dbContextFactory)
        {
            _logger = logger;
            _bluetoothService = bluetoothService;
            _dbContextFactory = dbContextFactory;
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            while (!stoppingToken.IsCancellationRequested)
            {
                await PollDevicesAsync();
                await Task.Delay(_pollingInterval, stoppingToken);
            }
        }

        private async Task PollDevicesAsync()
        {
            _logger.LogInformation("Polling devices for moisture data...");

            try
            {
                // 1. Scan for BLE devices
                var devices = await _bluetoothService.ScanDevicesAsync();

                // 2. Open a new DB context for each polling run
                using var dbContext = _dbContextFactory.CreateDbContext();

                // 3. Filter devices that have "SoilSensor" in the name (if desired)
                var soilDevices = devices
                    .Where(d => !string.IsNullOrEmpty(d.Name) && d.Name.Contains("SoilSensor", StringComparison.OrdinalIgnoreCase))
                    .ToList();

                // 4. For each device, attempt to get moisture data and store/update in DB
                foreach (var device in soilDevices)
                {
                    var existingDevice = dbContext.Devices
                        .FirstOrDefault(d => d.Uuid == device.Id.ToString());

                    if (existingDevice == null)
                    {
                        // Add new device to DB
                        existingDevice = new Device
                        {
                            Name = device.Name,
                            Nickname = "Pakkun Flower",
                            Uuid = device.Id.ToString(),
                            LastSeen = DateTime.Now
                        };
                        dbContext.Devices.Add(existingDevice);
                    }
                    else
                    {
                        existingDevice.LastSeen = DateTime.Now;
                    }

                    try
                    {
                        // Attempt to read moisture from the device
                        var moisture = await _bluetoothService.GetSoilMoistureAsync(device.Name);
                        existingDevice.Moisture = moisture;
                    }
                    catch (Exception ex)
                    {
                        // If reading fails, log and continue
                        _logger.LogWarning(ex, $"Failed to read moisture from {device.Name}");
                    }
                }

                // 5. Save changes
                await dbContext.SaveChangesAsync();
                _logger.LogInformation("Polling cycle complete.");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error during device polling.");
            }
        }
    }
}
