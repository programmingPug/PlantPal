
<br/>
<div align="center">
<a href="https://github.com/programmingPug/plantpal">
<img src="https://imgur.com/BdczfrQ.png" alt="Logo" width="80" height="80">
</a>
<h3 align="center">PlantPal</h3>
<p align="center">
Because knowing when to water your plants is hard
</p>
</div>

## Reporter node's features

### Captive Portal Configuration:

- Uses WiFiManager to allow a user to configure the Wi‑Fi credentials, API server URL, and scanning delay via a captive portal.
- A configuration mode is triggered automatically if no valid configuration is available or by user intervention via a dedicated button.

### BLE Scanning:

- Scans for BLE devices advertising a specific service UUID.
- Collects all sensor BLE addresses over a fixed scan period.

### Sequential Sensor Processing:

- Processes each discovered sensor one-by-one.
- For each sensor, the node:
1) Connects to the sensor via its BLE address.
2) Reads the sensor value from a characteristic.
3) Disconnects from the sensor.

### API Reporting:

- Constructs a JSON payload with the sensor’s BLE address as the device identifier, along with the measured moisture value.
- The payload is formatted according to a flattened model (including fields such as "deviceId", "moistureLevel", and a nested "device" object with "deviceId" and "nickname").
- Sends the payload via an HTTP POST request to your configured API endpoint.

### Configurable Scan Delay:

- Uses a user-configurable delay (default 30 seconds) between scan cycles so that after processing sensors, the node waits before scanning again.

### Robust Error Handling & State Management:

- Employs a simple state machine to manage scanning, connecting, and waiting phases.
- Includes duplicate filtering so the same sensor isn’t processed multiple times in one cycle.
- Provides conditional debug logging to aid in troubleshooting and can be disabled for production builds.

### Watchdog Integration:

- Uses built-in ESP32 Wachdog methods to ensure continued device operation.