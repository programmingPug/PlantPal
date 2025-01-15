#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
#include "esp_sleep.h"

// ***** Configuration *****
// Pins:
#define BATTERY_PIN A0        // Battery voltage reading via a resistor divider
#define SOIL_PIN    A2        // Soil moisture sensor analog reading
#define BUTTON_PIN  D1         // Button on D1, active LOW (internal pull-up)

// EEPROM settings: 4 bytes (2 for dry, 2 for wet)
#define EEPROM_SIZE 4
#define EEPROM_DRY_ADDR 0
#define EEPROM_WET_ADDR 2

// Sampling settings:
#define BATTERY_SAMPLES 10
#define SOIL_SAMPLES    20
#define SAMPLE_INTERVAL 100   // 0.1 seconds between soil samples
#define NORMAL_DELAY    2000  // 2 seconds delay between normal readings
#define CAL_DELAY       10000 // 10-second delay before calibration reading

// Deep sleep settings:
#define SLEEP_DURATION_HOURS 6
#define AWAKE_MAX_MS 300000   // 5 minutes in milliseconds

// Battery voltage parameters (assuming 220kΩ divider, two equal resistors)
#define ADC_MAX 4095.0
#define VREF    3.3
#define VOLTAGE_DIVIDER_FACTOR 2.0
#define BATTERY_FULL 4.2
#define BATTERY_EMPTY 3.0

// ***** BLE Settings *****
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define MOISTURE_CHAR_UUID  "abcd1234-5678-1234-5678-abcdef123456"
#define BATTERY_CHAR_UUID   "fedcba98-7654-3210-0123-456789abcdef"

// ***** Default Calibration Values *****
// These defaults are used if no valid calibration exists in EEPROM.
#define DEFAULT_DRY_CAL 3000  // Higher raw reading for dry soil
#define DEFAULT_WET_CAL 1000  // Lower raw reading for wet soil

// ***** Global Calibration Variables *****
int dryCal = 0;
int wetCal = 0;

// Calibration mode states
enum CalState {
  CAL_OFF,
  CAL_DRY_PENDING,
  CAL_WET_PENDING,
  CAL_DONE
};
CalState calState = CAL_OFF;

// Debounce for button on BUTTON_PIN
bool buttonPressed = false;
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 50; // 50 ms

// Operation mode: NORMAL_MODE or CALIBRATION_MODE
enum OperationMode {
  NORMAL_MODE,
  CALIBRATION_MODE
};
OperationMode opMode = NORMAL_MODE;

// ***** BLE Server Variables *****
BLEServer* pServer = nullptr;
BLECharacteristic* pMoistureCharacteristic = nullptr;
BLECharacteristic* pBatteryCharacteristic = nullptr;

// ***** Time Tracking *****
unsigned long awakeStartTime = 0;

// ***** Helper Functions *****

// Read battery voltage: average BATTERY_SAMPLES, convert reading to voltage, then map to a percentage.
float readBatteryVoltage() {
  long total = 0;
  for (int i = 0; i < BATTERY_SAMPLES; i++) {
    total += analogRead(BATTERY_PIN);
    delay(10);
  }
  float avg = total / (float)BATTERY_SAMPLES;
  float v_adc = (avg * VREF) / ADC_MAX;
  float v_bat = v_adc * VOLTAGE_DIVIDER_FACTOR;
  return v_bat;
}

int batteryPercentage(float voltage) {
  int percent = (int)(((voltage - BATTERY_EMPTY) / (BATTERY_FULL - BATTERY_EMPTY)) * 100);
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return percent;
}

// Read soil moisture raw value: average SOIL_SAMPLES spaced by SAMPLE_INTERVAL ms.
int readSoilRaw() {
  long sum = 0;
  for (int i = 0; i < SOIL_SAMPLES; i++) {
    sum += analogRead(SOIL_PIN);
    delay(SAMPLE_INTERVAL);
  }
  return sum / SOIL_SAMPLES;
}

// Map raw soil moisture value to percentage using calibration values.
int mapSoilMoisture(int raw) {
  if (dryCal == wetCal) return 0;
  int percent = map(raw, dryCal, wetCal, 0, 100);
  percent = constrain(percent, 0, 100);
  return percent;
}

// Update BLE characteristics with current battery and soil moisture data.
void updateBLE() {
  int soilRaw = readSoilRaw();
  int moisturePercent = 0;
  if (dryCal != 0 && wetCal != 0 && dryCal > wetCal) {
    moisturePercent = mapSoilMoisture(soilRaw);
  }
  
  float v_bat = readBatteryVoltage();
  int batPercent = batteryPercentage(v_bat);
  
  String moistureStr = String(moisturePercent);
  String batteryStr = String(batPercent);
  
  pMoistureCharacteristic->setValue(moistureStr.c_str());
  pMoistureCharacteristic->notify();
  
  pBatteryCharacteristic->setValue(batteryStr.c_str());
  pBatteryCharacteristic->notify();
  
  Serial.print("Battery: ");
  Serial.print(v_bat);
  Serial.print(" V (");
  Serial.print(batPercent);
  Serial.println("%)");
  Serial.print("Soil raw: ");
  Serial.print(soilRaw);
  Serial.print("  Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");
}

// Enter deep sleep for SLEEP_DURATION_HOURS hours.
void enterDeepSleep() {
  Serial.println("Entering deep sleep.");
  uint64_t sleepTimeUs = SLEEP_DURATION_HOURS * 3600ULL * 1000000ULL;
  esp_deep_sleep(sleepTimeUs);
}

// ----- Calibration Functions -----
// Wait CAL_DELAY ms then take SOIL_SAMPLES samples and return the average raw value.
int calibrateSensor() {
  delay(CAL_DELAY);
  return readSoilRaw();
}

// Check the state of the calibration button on BUTTON_PIN and update calibration state.
void checkButton() {
  if (digitalRead(BUTTON_PIN) == LOW) {  // Button pressed
    if (!buttonPressed && (millis() - lastButtonPress > debounceDelay)) {
      buttonPressed = true;
      lastButtonPress = millis();
      
      // Cycle through calibration states
      if (calState == CAL_OFF) {
        calState = CAL_DRY_PENDING;
        Serial.println("Button pressed: Entering Dry Calibration Mode. Ensure sensor is in dry soil.");
      } else if (calState == CAL_DRY_PENDING) {
        calState = CAL_WET_PENDING;
        Serial.println("Button pressed: Entering Wet Calibration Mode. Place sensor in wet soil.");
      } else if (calState == CAL_WET_PENDING) {
        calState = CAL_DONE;
        Serial.println("Button pressed: Finalizing Calibration.");
      }
    }
  } else {
    buttonPressed = false;
  }
}

// Runs the calibration sequence.
// In Dry mode: after delay, take sample and assign dryCal.
// In Wet mode: similarly assign wetCal.
void runCalibration() {
  if (calState == CAL_DRY_PENDING) {
    Serial.println("Starting Dry Calibration...");
    int dryValue = calibrateSensor();
    dryCal = dryValue;
    Serial.print("Dry calibration value: ");
    Serial.println(dryCal);
    // Wait for next button press to proceed to wet calibration.
    while (calState == CAL_DRY_PENDING) {
      checkButton();
      delay(100);
    }
  }
  if (calState == CAL_WET_PENDING) {
    Serial.println("Starting Wet Calibration...");
    int wetValue = calibrateSensor();
    wetCal = wetValue;
    Serial.print("Wet calibration value: ");
    Serial.println(wetCal);
    while (calState == CAL_WET_PENDING) {
      checkButton();
      delay(100);
    }
  }
  if (calState == CAL_DONE) {
    // Save calibration values to EEPROM
    EEPROM.write(EEPROM_DRY_ADDR, lowByte(dryCal));
    EEPROM.write(EEPROM_DRY_ADDR + 1, highByte(dryCal));
    EEPROM.write(EEPROM_WET_ADDR, lowByte(wetCal));
    EEPROM.write(EEPROM_WET_ADDR + 1, highByte(wetCal));
    EEPROM.commit();
    Serial.println("Calibration saved. Exiting calibration mode.");
    calState = CAL_OFF;
    opMode = NORMAL_MODE;
  }
}

// ----- BLE Server Setup -----
void setupBLEServer() {
  BLEDevice::init("SoilSensor_XIAO");
  pServer = BLEDevice::createServer();
  BLEService* pService = pServer->createService(SERVICE_UUID);
  
  pMoistureCharacteristic = pService->createCharacteristic(
                              MOISTURE_CHAR_UUID,
                              BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                              );
  pMoistureCharacteristic->setValue("0");
  
  pBatteryCharacteristic = pService->createCharacteristic(
                              BATTERY_CHAR_UUID,
                              BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                              );
  pBatteryCharacteristic->setValue("0");
  
  pService->start();
  
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE Server started and advertising.");
}

// ----- Main Setup and Loop -----

void setup() {
  Serial.begin(115200);
  Serial.println("Booting Sensor Node...");

  EEPROM.begin(EEPROM_SIZE);
  
  pinMode(BATTERY_PIN, INPUT);
  pinMode(SOIL_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize BLE Server so sensor values can be advertised
  setupBLEServer();
  
  // Load calibration from EEPROM
  int storedDry = EEPROM.read(EEPROM_DRY_ADDR) | (EEPROM.read(EEPROM_DRY_ADDR + 1) << 8);
  int storedWet = EEPROM.read(EEPROM_WET_ADDR) | (EEPROM.read(EEPROM_WET_ADDR + 1) << 8);
  // Check validity: dry value should be higher than wet value and nonzero.
  if (storedDry > storedWet && storedDry > 0 && storedWet > 0) {
    dryCal = storedDry;
    wetCal = storedWet;
    Serial.print("Loaded Calibration - Dry: ");
    Serial.print(dryCal);
    Serial.print("  Wet: ");
    Serial.println(wetCal);
    opMode = NORMAL_MODE;
  } else {
    // Use default calibration values if none are valid.
    dryCal = DEFAULT_DRY_CAL;
    wetCal = DEFAULT_WET_CAL;
    Serial.println("No valid calibration found. Using default calibration values.");
    Serial.print("Default Dry: ");
    Serial.print(dryCal);
    Serial.print("  Default Wet: ");
    Serial.println(wetCal);
    opMode = NORMAL_MODE; // Allow device to be usable even without calibration.
  }
  
  awakeStartTime = millis();
}

void loop() {
  // Check for button press to trigger calibration mode.
  checkButton();
  if (digitalRead(BUTTON_PIN) == LOW && opMode == NORMAL_MODE) {
    // If button is pressed while in normal mode, enter calibration mode.
    Serial.println("Button pressed while awake. Entering Calibration Mode.");
    opMode = CALIBRATION_MODE;
    calState = CAL_DRY_PENDING;
  }
  
  if (opMode == CALIBRATION_MODE) {
    runCalibration();
    awakeStartTime = millis();  // reset awake time after calibration
    return;
  }
  
  // NORMAL MODE
  updateBLE();
  
  // If more than 5 minutes have elapsed, go to deep sleep for 6 hours.
  if (millis() - awakeStartTime >= AWAKE_MAX_MS) {
    Serial.println("5 minutes elapsed; going to deep sleep.");
    enterDeepSleep();
  }
  
  delay(NORMAL_DELAY);
}
