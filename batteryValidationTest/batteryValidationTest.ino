// XAIO-ESP32-C6 Battery Monitor with Averaged ADC Readings
// Measures the battery voltage via a resistor divider (two 220 kΩ resistors),
// averages 10 ADC readings, and reports the battery percentage.
// Battery assumed: single-cell LiPo with 3.0V (0%) to 4.2V (100%).

// ----- CONFIGURATION -----
const int analogPin = A0;             // ADC pin connected to the resistor divider
const float adcRefVoltage = 3.3;        // ADC reference voltage (Volts)
const int adcMaxValue = 4095;           // Maximum ADC value (for 12-bit resolution)
const float voltageDividerFactor = 2.0; // Divider factor: battery voltage is divided by 2

// Battery voltage range settings (adjust as necessary)
const float batteryMinVoltage = 3.0;    // Voltage corresponding to 0% battery
const float batteryMaxVoltage = 4.2;    // Voltage corresponding to 100% battery

const int numSamples = 10;              // Number of ADC samples to average

// ----- SETUP -----
void setup() {
  Serial.begin(115200);
  // If needed, additional ADC calibration or configuration can be added here
}

// ----- MAIN LOOP -----
void loop() {
  long totalADC = 0;
  
  // Take numSamples readings
  for (int i = 0; i < numSamples; i++) {
    totalADC += analogRead(analogPin);
    delay(10); // Short delay between samples (adjust as needed)
  }
  
  // Calculate the average ADC value
  float avgADC = totalADC / (float)numSamples;
  
  // Convert the averaged ADC value to the measured voltage at the divider
  float measuredVoltage = (avgADC / adcMaxValue) * adcRefVoltage;
  
  // Calculate the actual battery voltage using the divider factor
  float batteryVoltage = measuredVoltage * voltageDividerFactor;
  
  // Map the battery voltage to a battery percentage using a linear mapping
  float batteryPercent = ((batteryVoltage - batteryMinVoltage) / (batteryMaxVoltage - batteryMinVoltage)) * 100.0;
  if (batteryPercent > 100.0) batteryPercent = 100.0;
  if (batteryPercent < 0.0)   batteryPercent = 0.0;
  
  // Print the results to the Serial Monitor
  Serial.print("Avg ADC Value: ");
  Serial.print(avgADC, 2);
  Serial.print(" | Divider Voltage: ");
  Serial.print(measuredVoltage, 2);
  Serial.print(" V | Battery Voltage: ");
  Serial.print(batteryVoltage, 2);
  Serial.print(" V | Battery Percent: ");
  Serial.print(batteryPercent, 1);
  Serial.println("%");
  
  // Wait 2 seconds before the next reading
  delay(2000);
}
