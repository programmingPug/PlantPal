namespace house_plant_api.Services
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using house_plant_api.Models;
    using InTheHand.Bluetooth;

    public class BluetoothService
    {
        private readonly Guid _serviceUuid = Guid.Parse("12345678-1234-1234-1234-123456789abc");
        private readonly Guid _characteristicUuid = Guid.Parse("abcd1234-5678-1234-5678-abcdef123456");

        public async Task<IReadOnlyCollection<BluetoothDevice>> ScanDevicesAsync()
        {
            // Scan for BLE devices
            return await Bluetooth.ScanForDevicesAsync();
        }

        public async Task<int> GetSoilMoistureAsync(string deviceName)
        {
            // 1. Scan for devices and match by name
            var devices = await Bluetooth.ScanForDevicesAsync();
            var targetDevice = devices.FirstOrDefault(d => d.Name == deviceName);

            if (targetDevice == null)
                throw new Exception($"Device '{deviceName}' not found.");

            // 2. Connect and read moisture
            await targetDevice.Gatt.ConnectAsync();
            try
            {
                var services = await targetDevice.Gatt.GetPrimaryServicesAsync();
                var targetService = services.FirstOrDefault(s => s.Uuid == _serviceUuid);
                if (targetService == null)
                    throw new Exception("Moisture service not found.");

                var characteristics = await targetService.GetCharacteristicsAsync();
                var targetCharacteristic = characteristics.FirstOrDefault(c => c.Uuid == _characteristicUuid);
                if (targetCharacteristic == null)
                    throw new Exception("Moisture characteristic not found.");

                var value = await targetCharacteristic.ReadValueAsync();
                return BitConverter.ToInt32(value, 0);
            }
            finally
            {
                targetDevice.Gatt.Disconnect();
            }
        }
    }
}
