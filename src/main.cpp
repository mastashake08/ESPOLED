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

// Forward declaration
void updateDisplay(String message);

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
      String value = pCharacteristic->getValue().c_str();

      if (value.length() > 0) {
        receivedMessage = value;
        Serial.print("Received: ");
        Serial.println(receivedMessage);
        
        // Update display with received message
        updateDisplay(receivedMessage);
      }
    }
};

// Function declarations
void initOLED();
void initBLE();

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial time to initialize
  Serial.println("\n\n=================================");
  Serial.println("ESP32 BLE OLED Server Starting...");
  Serial.println("=================================");

  // Initialize OLED display
  initOLED();
  
  // Initialize BLE
  initBLE();
  
  Serial.println("\n=================================");
  Serial.println("System Ready!");
  Serial.println("- Send text via BLE or Serial Monitor");
  Serial.println("- Type text and press Enter to display");
  Serial.println("=================================\n");
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    String serialMessage = Serial.readStringUntil('\n');
    serialMessage.trim(); // Remove whitespace/newlines
    
    if (serialMessage.length() > 0) {
      Serial.print("Serial input received: ");
      Serial.println(serialMessage);
      updateDisplay(serialMessage);
    }
  }
  
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
  // Initialize I2C with explicit pins (GPIO21=SDA, GPIO22=SCL)
  Wire.begin(21, 22);
  delay(100);
  
  Serial.println("Scanning I2C bus...");
  byte error, address;
  int nDevices = 0;
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found!");
  }
  
  Serial.print("Attempting to initialize display at address 0x");
  Serial.println(SCREEN_ADDRESS, HEX);
  
  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    Serial.println(F("Check:"));
    Serial.println(F("- I2C address (try 0x3D if 0x3C fails)"));
    Serial.println(F("- Wiring: SDA->GPIO21, SCL->GPIO22"));
    Serial.println(F("- Display power connection"));
    Serial.println(F("Continuing without display..."));
    return; // Continue without display instead of halting
  }
  
  Serial.println("Display initialized successfully!");
  
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
  
  Serial.println("OLED Display Ready");
}

void initBLE() {
  // Create BLE Device
  BLEDevice::init("MORIAH_ESP");

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
  
  Serial.println("BLE Service Started - Device Name: MORIAH_ESP");
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