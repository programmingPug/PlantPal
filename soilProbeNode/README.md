
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

## Probe node's features

### Battery Monitoring:

- The probe node reads the battery voltage from the analog input (A0) using 10 samples and calculates an average.
- Using a resistor divider (220 kΩ resistors) and known voltage parameters, it converts the ADC reading into an estimated battery voltage.
- The voltage is then mapped to a percentage (for a typical 3.7V LiPo, for example, with 4.2 V as 100% and 3.0 V as 0%) so that you always know the current battery state.

### Soil Moisture Sensing:

- The sensor on A3 is read using 20 samples spaced 0.1 seconds apart. This creates an average raw reading that is then mapped into a moisture percentage.
- Calibration values (dry and wet) are used to translate the raw sensor value into a meaningful moisture percentage.

### Calibration Mode:

- A physical button on D1 is used to trigger calibration.
1) First press: Enters dry calibration mode. After a 10‑second delay, the device takes 20 samples and averages them to determine the "dry" value.
2) Second press: Enters wet calibration mode with the same process to record the "wet" value.
3) Third press: Finalizes calibration, saves the new calibration values to EEPROM, and exits calibration mode.
- If no user calibration has been performed (or if no valid calibration is present in EEPROM), the node falls back on predefined default calibration values so that it is still usable.

### BLE Server & Advertising:

- The probe node sets up a BLE server using the predefined service and characteristic UUIDs.
- It advertises its data (both soil moisture and battery percentage) as BLE characteristics so that a reporter node can connect, read, and process the data.
- The BLE characteristics are updated periodically with the latest sensor readings and notify connected clients.

### Deep Sleep for Energy Efficiency:

- For battery-powered operation, once data is reported (or if the device has been awake for a set period), the probe node enters deep sleep (for example, 6 hours). This helps to conserve battery life, yet the device can be woken up—by a button press—for calibration or other interactions.

### Overall Integration:

- The node uses EEPROM to store calibration data so that values persist across power cycles.
- The code is structured to average multiple readings (to avoid outliers) and uses delay intervals that ensure stable sensor data.
- Debug output via Serial is available (and can be disabled for production) to assist during development and troubleshooting.