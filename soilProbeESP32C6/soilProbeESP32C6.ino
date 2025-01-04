#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <EEPROM.h>
#include <esp_sleep.h>

// ======== BLE UUIDs ========
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"

// ======== Hardware & ADC ========
#define MOISTURE_PIN A0
#define LED_PIN      2 // Example LED pin (built-in LED on many ESP32 boards)
#define BUTTON_PIN   D4 // GPIO 4 for calibration button (recommended over GPIO 0)

// ======== Constants ========
const int DRY_VALUE_DEFAULT = 500;
const int WET_VALUE_DEFAULT = 200;
const int ADC_RESOLUTION = 10;  // 10-bit ADC (0–1023)

// ======== Reading & Averaging ========
const int NUM_SAMPLES = 5;        // Number of samples to average
const int SAMPLE_DELAY_MS = 50;   // Delay between each sample (ms)

// ======== Connection Timeout Settings ========
const unsigned long CONNECTION_TIMEOUT_MS = 120000; // 2 minutes to establish connection
unsigned long connectionStartTime = 0;
bool connectionTimerStarted = false;

// ======== Sleep Settings ========
const uint64_t DEEP_SLEEP_DURATION_US = 60ULL * 60ULL * 1000000ULL; // 1 hour in microseconds

// ======== Calibration Variables ========
int dryValue = DRY_VALUE_DEFAULT; // Initialize with default dry value
int wetValue = WET_VALUE_DEFAULT; // Initialize with default wet value

// ======== Device Configuration ========
const char* deviceName = "003-SoilSensor"; // Default device name

// ======== EEPROM Addresses ========
const int EEPROM_SIZE = 512;
const int EEPROM_DRY_ADDRESS = 0;  // Bytes 0-3
const int EEPROM_WET_ADDRESS = 4;  // Bytes 4-7

// ======== BLE Objects ========
BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;

// ======== Device State Flags ========
bool isConnected = false;
bool dataSent = false;

// ======== Calibration Variables ========
enum DeviceState {
  STATE_NORMAL,
  STATE_CALIBRATE_NOT_IN_SOIL,
  STATE_CALIBRATE_IN_SOIL
};

DeviceState currentState = STATE_NORMAL;
int calibrationCount = 0;

// ======== Button Handling ========
const unsigned long DEBOUNCE_DELAY = 50; // 50ms debounce delay
bool lastButtonState = HIGH;              // Assuming pull-up resistor
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;

// ======== LED Blinking Parameters ========
unsigned long previousBlinkTime = 0;
const unsigned long BLINK_ON_DURATION = 200;  // 200ms ON
const unsigned long BLINK_OFF_DURATION = 100; // 100ms OFF
bool ledState = false;

// ======== Function Prototypes ========
int readSoilMoisture();
int getAveragedMoisture(int numSamples);
void enterDeepSleep();
void handleCalibrationButtonPress();
void readCalibrationValues();
void writeCalibrationValues(int dry, int wet);
void checkButtonPress();
void updateLED();

// ======== Custom Server Callbacks ========
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    Serial.println("Client connected.");
    isConnected = true;

    // Turn on LED to indicate active state
    digitalWrite(LED_PIN, HIGH);

    // Stop connection timeout timer
    connectionTimerStarted = false;
    connectionStartTime = 0;
  }

  void onDisconnect(BLEServer* pServer) override {
    Serial.println("Client disconnected.");
    isConnected = false;
    dataSent = false;

    // Restart advertising to allow new connections
    pServer->startAdvertising();

    // Restart connection timeout timer
    connectionTimerStarted = true;
    connectionStartTime = millis();

    // Turn off LED to indicate sleep state
    digitalWrite(LED_PIN, LOW);
  }
};

// ======== Sensor Reading Functions ========
/**
 * Perform a single soil moisture reading (0–100%).
 */
int readSoilMoisture() {
  int rawValue = analogRead(MOISTURE_PIN);
  int percent  = map(rawValue, dryValue, wetValue, 0, 100);
  return constrain(percent, 0, 100);
}

/**
 * Gathers multiple samples, applies a small delay between each,
 * averages them, and returns the final moisture value.
 */
int getAveragedMoisture(int numSamples) {
  long sum = 0;

  for (int i = 0; i < numSamples; i++) {
    sum += readSoilMoisture();
    delay(SAMPLE_DELAY_MS);  // Small delay between samples
  }

  int avg = (int)(sum / numSamples);
  return avg;
}

/**
 * Enter deep sleep mode for the defined duration.
 */
void enterDeepSleep() {
  Serial.println("Entering deep sleep for 1 hour...");

  // Turn off LED to indicate sleep state
  digitalWrite(LED_PIN, LOW);
  
  // Disable BLE before sleeping
  BLEDevice::deinit(true);

  // Configure deep sleep wake-up time
  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_DURATION_US);

  // Enter deep sleep
  esp_deep_sleep_start();
}

/**
 * Handle calibration button presses to manage calibration steps.
 */
void handleCalibrationButtonPress() {
  calibrationCount++;

  switch (calibrationCount) {
    case 1:
      // Calibration Step 1: Record Dry Value
      Serial.println("Calibration Step 1: Place device in dry soil and press button.");
      currentState = STATE_CALIBRATE_NOT_IN_SOIL;
      break;

    case 2:
      // Calibration Step 2: Record Wet Value
      Serial.println("Calibration Step 2: Place device in wet soil and press button.");
      currentState = STATE_CALIBRATE_IN_SOIL;
      break;

    case 3:
      // Calibration Complete
      Serial.println("Calibration Complete. Returning to normal operation.");
      currentState = STATE_NORMAL;
      calibrationCount = 0; // Reset for next calibration
      break;

    default:
      // Reset Calibration
      Serial.println("Calibration Reset. Returning to normal operation.");
      currentState = STATE_NORMAL;
      calibrationCount = 0;
      break;
  }

  updateLED();
}

/**
 * Read calibration values from EEPROM.
 */
void readCalibrationValues() {
  EEPROM.get(EEPROM_DRY_ADDRESS, dryValue);
  EEPROM.get(EEPROM_WET_ADDRESS, wetValue);
  
  // Validate calibration values
  if (dryValue == 0xFFFFFFFF || wetValue == 0xFFFFFFFF) {
    dryValue = DRY_VALUE_DEFAULT;
    wetValue = WET_VALUE_DEFAULT;
  }
}

/**
 * Write calibration values to EEPROM.
 */
void writeCalibrationValues(int dry, int wet) {
  EEPROM.put(EEPROM_DRY_ADDRESS, dry);
  EEPROM.put(EEPROM_WET_ADDRESS, wet);
  EEPROM.commit();
}

/**
 * Check for button presses with debouncing.
 */
void checkButtonPress() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading == LOW && lastButtonState == HIGH) {
      buttonPressed = true;
    }
  }

  lastButtonState = reading;
}

/**
 * Update LED based on the current state.
 */
void updateLED() {
  // Reset LED parameters
  previousBlinkTime = millis();
  ledState = false;

  switch (currentState) {
    case STATE_NORMAL:
      // LED is handled by BLE connection callbacks
      digitalWrite(LED_PIN, LOW); // Ensure LED is off in normal mode
      break;

    case STATE_CALIBRATE_NOT_IN_SOIL:
      // Solid LED to indicate calibration for dry soil
      digitalWrite(LED_PIN, HIGH);
      break;

    case STATE_CALIBRATE_IN_SOIL:
      // Start blinking in 2:1 ratio for wet soil calibration
      digitalWrite(LED_PIN, HIGH); // Start with LED ON
      ledState = true;
      break;
  }
}

/**
 * Initialize EEPROM and read calibration values.
 */
void initializeEEPROM() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialize EEPROM");
    // Handle EEPROM initialization failure if necessary
  }
  readCalibrationValues();
}

/**
 * Update mapping based on calibrated dry and wet values.
 */
int mapMoisture(int rawValue) {
  int percent = map(rawValue, dryValue, wetValue, 0, 100);
  return constrain(percent, 0, 100);
}

// ======== Setup Function ========
void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(1000); // Allow time for serial to initialize

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED off initially

  // Initialize Button pin
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use INPUT_PULLUP if using internal resistor
  // If using external resistor, use INPUT instead:
  // pinMode(BUTTON_PIN, INPUT);

  // Initialize EEPROM
  initializeEEPROM();
  Serial.printf("Calibrated Dry Value: %d\n", dryValue);
  Serial.printf("Calibrated Wet Value: %d\n", wetValue);

  // Initialize BLE with deviceName variable
  BLEDevice::init(deviceName); // Initialize BLE with the variable device name
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create BLE characteristic
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  // Add descriptor for notifications
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->setScanResponse(true);
  pServer->getAdvertising()->start();

  Serial.println("BLE server is running...");

  // Start connection timeout timer
  connectionTimerStarted = true;
  connectionStartTime = millis();

  // Initialize state and LED
  currentState = STATE_NORMAL;
  calibrationCount = 0;
  updateLED();
}

// ======== Loop Function ========
void loop() {
  // Check for button presses
  checkButtonPress();

  if (buttonPressed) {
    buttonPressed = false;
    if (currentState != STATE_NORMAL) {
      // Handle calibration step
      handleCalibrationButtonPress();
    } else {
      // Initiate calibration
      Serial.println("Button pressed. Initiating calibration mode.");
      currentState = STATE_CALIBRATE_NOT_IN_SOIL;
      calibrationCount = 1;
      updateLED();
    }
  }

  unsigned long currentTime = millis();

  // ======== Connection Timeout Logic ========
  if (connectionTimerStarted) {
    if (currentTime - connectionStartTime >= CONNECTION_TIMEOUT_MS) {
      Serial.println("No connection within 2 minutes. Entering deep sleep.");
      enterDeepSleep();
    }
  }

  // ======== BLE Connected Logic ========
  if (isConnected && !dataSent && currentState == STATE_NORMAL) {
    // Take sensor readings
    int rawMoisture = analogRead(MOISTURE_PIN);
    int moistureAvg = mapMoisture(rawMoisture);
    Serial.printf("Averaged Soil Moisture: %d%%\n", moistureAvg);

    // Update the characteristic and notify the client
    pCharacteristic->setValue(moistureAvg);
    pCharacteristic->notify();

    dataSent = true;

    // Immediately enter deep sleep after sending data
    enterDeepSleep();
  }

  // ======== LED Update ========
  updateLED();

  // ======== LED Blinking Logic ========
  if (currentState == STATE_CALIBRATE_IN_SOIL) {
    if (millis() - previousBlinkTime >= (ledState ? BLINK_ON_DURATION : BLINK_OFF_DURATION)) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      previousBlinkTime = millis();
    }
  }

  // Short delay to prevent watchdog resets
  delay(100);
}
