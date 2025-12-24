#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // Common I2C address for SSD1306

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// BLE Configuration
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
String receivedMessage = "";

// BLE Server Callbacks
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("BLE Client Connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("BLE Client Disconnected");
    }
};

// BLE Characteristic Callbacks
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        receivedMessage = String(value.c_str());
        Serial.print("Received: ");
        Serial.println(receivedMessage);
        
        // Update display with received message
        updateDisplay(receivedMessage);
      }
    }
};

// Function declarations
void updateDisplay(String message);
void initOLED();
void initBLE();

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 BLE OLED Server Starting...");

  // Initialize OLED display
  initOLED();
  
  // Initialize BLE
  initBLE();
  
  Serial.println("System Ready - Waiting for BLE connections...");
}

void loop() {
  // Handle BLE connection state changes
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // Give the bluetooth stack time to get ready
    pServer->startAdvertising(); // Restart advertising
    Serial.println("Restarting BLE advertising");
    oldDeviceConnected = deviceConnected;
  }
  
  // Handle new connections
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
  
  delay(10);
}

void initOLED() {
  // Initialize I2C with default pins (GPIO21=SDA, GPIO22=SCL)
  Wire.begin();
  
  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Halt if display init fails
  }
  
  // Clear display and show startup message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("BLE OLED Server"));
  display.println(F(""));
  display.println(F("Waiting for"));
  display.println(F("connection..."));
  display.display();
  
  Serial.println("OLED Display Initialized");
}

void initBLE() {
  // Create BLE Device
  BLEDevice::init("ESP32_OLED");

  // Create BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  // Add descriptor for notifications (optional, for future enhancements)
  pCharacteristic->addDescriptor(new BLE2902());
  
  // Set callbacks
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // For iPhone connection issues
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE Service Started - Device Name: ESP32_OLED");
  Serial.print("Service UUID: ");
  Serial.println(SERVICE_UUID);
}

void updateDisplay(String message) {
  display.clearDisplay();
  display.setTextSize(2);  // Larger text for messages
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  // Word wrap for long messages
  int charWidth = 6 * 2;  // Character width * text size
  int maxCharsPerLine = SCREEN_WIDTH / charWidth;
  int lineHeight = 16;  // Height per line with text size 2
  int currentLine = 0;
  int maxLines = SCREEN_HEIGHT / lineHeight;
  
  int startIdx = 0;
  while (startIdx < message.length() && currentLine < maxLines) {
    int endIdx = min(startIdx + maxCharsPerLine, (int)message.length());
    
    // Try to break at space if possible
    if (endIdx < message.length()) {
      int lastSpace = message.lastIndexOf(' ', endIdx);
      if (lastSpace > startIdx) {
        endIdx = lastSpace;
      }
    }
    
    String line = message.substring(startIdx, endIdx);
    display.setCursor(0, currentLine * lineHeight);
    display.println(line);
    
    startIdx = endIdx;
    if (startIdx < message.length() && message.charAt(startIdx) == ' ') {
      startIdx++; // Skip space at start of next line
    }
    currentLine++;
  }
  
  display.display();
}