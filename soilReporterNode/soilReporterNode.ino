#include <WiFiManager.h>      // ESP32 WiFiManager library for captive portal configuration
#include <WiFi.h>
#include <HTTPClient.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <vector>
using std::vector;

// --- Debug Logging: Uncomment to enable debugging ---
#define DEBUG 1
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Global configuration parameters
String apiServer = "http://192.168.1.193:8000/api/soilmoisture";  // Default API server URL
int scanningDelaySec = 30;  // Default scanning delay in seconds

// WiFiManager parameters for additional configuration
WiFiManagerParameter customServer("server", "API Server URL", apiServer.c_str(), 100);
WiFiManagerParameter customScanDelay("scandelay", "Scan Delay (sec)", String(scanningDelaySec).c_str(), 10);

// Create a WiFiManager instance
WiFiManager wifiManager;

// Global variable for the API key (modify as needed)
//const char* apiKey = "YOUR_UNIQUE_API_KEY_HERE";

// BLE configuration
#define SCAN_TIME 5  // Scan duration in seconds for each scan cycle
static BLEUUID serviceUUID("12345678-1234-1234-1234-123456789abc");
static BLEUUID charUUID("abcd1234-5678-1234-5678-abcdef123456");

// Global vector to store discovered sensor BLE addresses
vector<BLEAddress> foundSensors;

// State machine for controlling the reporter node
enum ReporterState {
  STATE_SCANNING,
  STATE_CONNECTING,
  STATE_WAITING
};
ReporterState currentState = STATE_SCANNING;
unsigned long waitStartTime = 0;

// Forward declarations of functions
bool connectToSensor(const BLEAddress &addr);
bool sendDataToAPI(const String &sensorAddress, float moistureValue);

// --- BLE Advertised Device Callback: Collect sensor addresses ---
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  public:
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
      if (advertisedDevice.haveServiceUUID() &&
          advertisedDevice.isAdvertisingService(serviceUUID)) {
        BLEAddress addr = advertisedDevice.getAddress();
        // Avoid duplicates
        bool alreadyAdded = false;
        for (auto &existing : foundSensors) {
          if (existing.equals(addr)) {
            alreadyAdded = true;
            break;
          }
        }
        if (!alreadyAdded) {
          DEBUG_PRINT("Found sensor: ");
          DEBUG_PRINTLN(addr.toString().c_str());
          foundSensors.push_back(addr);
        }
      }
    }
};

// --- Minimal BLE Client Callbacks ---
class MyClientCallbacks : public BLEClientCallbacks {
  public:
    void onConnect(BLEClient* pClient) override {
      DEBUG_PRINTLN("BLE Client connected.");
    }
    void onDisconnect(BLEClient* pClient) override {
      DEBUG_PRINTLN("BLE Client disconnected.");
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting Reporter Node with Captive Portal Configuration..."));

  // --- WiFiManager Captive Portal Configuration ---
  // Add custom parameters so the user can configure the API server URL and scan delay.
  wifiManager.addParameter(&customServer);
  wifiManager.addParameter(&customScanDelay);
  
  // Optionally force configuration mode (for example, via a hardware button or by calling resetSettings())
  // Uncomment the next line to force configuration mode for testing:
  //wifiManager.resetSettings();
  
  // Attempt autoConnect; if there’s no saved config, the captive portal AP "ConfigAP" (with password "configpassword") is created.
  if (!wifiManager.autoConnect("PlantPalConfig", "plantpal")) {
    DEBUG_PRINTLN("Failed to connect and hit timeout. Restarting...");
    delay(4000);
    ESP.restart();
  }
  Serial.println(F("WiFi connected via captive portal."));
  
  // Retrieve updated configuration parameters from WiFiManager:
  apiServer = String(customServer.getValue());
  scanningDelaySec = (customScanDelay.getValue() != nullptr) ? atoi(customScanDelay.getValue()) : scanningDelaySec;
  DEBUG_PRINT("Configured API Server: ");
  DEBUG_PRINTLN(apiServer);
  DEBUG_PRINT("Configured Scan Delay (sec): ");
  DEBUG_PRINTLN(scanningDelaySec);
  
  // --- Initialize BLE ---
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->start(SCAN_TIME, false);
  
  currentState = STATE_SCANNING;
}

void loop() {
  // Optionally, if you're using a hardware watchdog, call esp_task_wdt_reset() here.
  // esp_task_wdt_reset();

  switch(currentState) {
    case STATE_SCANNING: {
      // Start a new scan cycle
      BLEScan* pBLEScan = BLEDevice::getScan();
      pBLEScan->start(SCAN_TIME, false);
      pBLEScan->clearResults();
      if (!foundSensors.empty()) {
        currentState = STATE_CONNECTING;
        DEBUG_PRINTLN("Transitioning to STATE_CONNECTING");
      }
      break;
    }
      
    case STATE_CONNECTING: {
      if (!foundSensors.empty()) {
        BLEAddress addr = foundSensors.front();
        foundSensors.erase(foundSensors.begin());
        DEBUG_PRINT("Processing sensor: ");
        DEBUG_PRINTLN(addr.toString().c_str());
        if (connectToSensor(addr)) {
          DEBUG_PRINTLN("Sensor processed successfully.");
        } else {
          DEBUG_PRINTLN("Failed to process sensor.");
        }
      }
      // If no more sensors remain, switch to waiting state.
      if (foundSensors.empty()) {
        currentState = STATE_WAITING;
        waitStartTime = millis();
        DEBUG_PRINTLN("All sensors processed. Transitioning to STATE_WAITING.");
      }
      break;
    }
      
    case STATE_WAITING: {
      if (millis() - waitStartTime >= (unsigned long)scanningDelaySec * 1000) {
        currentState = STATE_SCANNING;
        DEBUG_PRINTLN("Transitioning to STATE_SCANNING");
        foundSensors.clear();
      }
      break;
    }
      
    default:
      break;
  }
  
  delay(10); // Small delay to yield time to other tasks
}

bool connectToSensor(const BLEAddress &addr) {
  BLEAddress sensorAddr = addr; // Make a local copy for printing
  DEBUG_PRINT("Connecting to sensor: ");
  DEBUG_PRINTLN(sensorAddr.toString().c_str());
  
  BLEClient* pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallbacks());
  
  if (!pClient->connect(sensorAddr)) {
    DEBUG_PRINTLN("Failed to connect.");
    pClient->disconnect();
    return false;
  }
  DEBUG_PRINTLN("Connected to sensor.");
  
  pClient->setMTU(517);  // Optionally set a larger MTU
  
  BLERemoteService* pService = pClient->getService(serviceUUID);
  if (pService == nullptr) {
    DEBUG_PRINT("Failed to find service UUID: ");
    DEBUG_PRINTLN(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  DEBUG_PRINTLN("Service found.");
  
  BLERemoteCharacteristic* pCharacteristic = pService->getCharacteristic(charUUID);
  if (pCharacteristic == nullptr) {
    DEBUG_PRINT("Failed to find characteristic UUID: ");
    DEBUG_PRINTLN(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  DEBUG_PRINTLN("Characteristic found.");
  
  if (pCharacteristic->canRead()) {
    String value = pCharacteristic->readValue();
    DEBUG_PRINT("Sensor Value: ");
    DEBUG_PRINTLN(value);
    
    float moistureValue = value.toFloat();
    if (sendDataToAPI(sensorAddr.toString(), moistureValue)) {
      DEBUG_PRINTLN("Data sent to API successfully.");
    } else {
      DEBUG_PRINTLN("Failed to send data to API.");
    }
  } else {
    DEBUG_PRINTLN("Characteristic not readable.");
  }
  
  pClient->disconnect();
  return true;
}

bool sendDataToAPI(const String &sensorAddress, float moistureValue) {
  if (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINTLN("Wi-Fi not connected.");
    return false;
  }
  
  HTTPClient http;
  http.begin(apiServer);
  http.addHeader("Content-Type", "application/json");
  //http.addHeader("X-API-KEY", apiKey);
  
  // Construct the JSON payload based on the model:
  // {
  //   "deviceId": "sensorAddress",
  //   "moistureLevel": moistureValue,
  //   "device": {
  //       "deviceId": "sensorAddress",
  //       "nickname": "SoilSensor",
  //       "soilMoistures": []
  //   }
  // }
  String payload = String("{\"deviceId\":\"")
                   + sensorAddress
                   + String("\",\"moistureLevel\":")
                   + String(moistureValue)
                   + String("}");
  
  DEBUG_PRINT("Sending payload: ");
  DEBUG_PRINTLN(payload);
  
  int httpResponseCode = http.POST(payload);
  DEBUG_PRINT("HTTP Response code: ");
  DEBUG_PRINTLN(httpResponseCode);
  String response = http.getString();
  DEBUG_PRINT("Response: ");
  DEBUG_PRINTLN(response);
  
  http.end();
  return (httpResponseCode > 0);
}
