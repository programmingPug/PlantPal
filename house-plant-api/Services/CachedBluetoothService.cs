namespace house_plant_api.Services
{
    using house_plant_api.Context;
    using house_plant_api.Models;
    using Microsoft.EntityFrameworkCore;
    using System;
    using System.Linq;
    using System.Threading.Tasks;

    public class CachedBluetoothService
    {
        private readonly BluetoothService _bluetoothService;
        private readonly IDbContextFactory<AppDbContext> _dbContextFactory;

        public CachedBluetoothService(
            BluetoothService bluetoothService,
            IDbContextFactory<AppDbContext> dbContextFactory)
        {
            _bluetoothService = bluetoothService;
            _dbContextFactory = dbContextFactory;
        }

        public List<Device> GetTrackedDevices()
        {
            // If you're using an IDbContextFactory:
            using var dbContext = _dbContextFactory.CreateDbContext();
            // Or if you're injecting a scoped dbContext (see note below):
            // var dbContext = _dbContext;

            return dbContext.Devices.ToList();
        }

        public async Task<List<Device>> GetTrackedDevicesAsync()
        {
            using var dbContext = _dbContextFactory.CreateDbContext();
            return await dbContext.Devices.ToListAsync();
        }

        public async Task PollDevicesAsync()
        {
            try
            {
                var devices = await _bluetoothService.ScanDevicesAsync();

                // Create a fresh AppDbContext instance
                using var dbContext = _dbContextFactory.CreateDbContext();

                foreach (var device in devices)
                {
                    var existingDevice = dbContext.Devices
                        .FirstOrDefault(d => d.Uuid == device.Id.ToString());

                    if (existingDevice == null)
                    {
                        dbContext.Devices.Add(new Device
                        {
                            Name = device.Name,
                            Nickname = "Pakkun Flower",
                            Uuid = device.Id.ToString(),
                            LastSeen = DateTime.Now
                        });
                    }
                    else
                    {
                        existingDevice.LastSeen = DateTime.Now;

                        try
                        {
                            var moisture = await _bluetoothService.GetSoilMoistureAsync(device.Name);
                            existingDevice.Moisture = moisture;
                        }
                        catch
                        {
                            // Swallow exceptions from reading moisture
                        }
                    }
                }

                await dbContext.SaveChangesAsync();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error during polling: {ex.Message}");
            }
        }

        public async Task<bool> DeleteDeviceAsync(string name)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();
            var device = dbContext.Devices.FirstOrDefault(d => d.Name == name);

            if (device == null)
                return false;

            dbContext.Devices.Remove(device);
            await dbContext.SaveChangesAsync();
            return true;
        }

    }
}
