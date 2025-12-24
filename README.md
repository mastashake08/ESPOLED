# ESPOLED - ESP32 BLE OLED Server

A Bluetooth Low Energy (BLE) server running on ESP32 that receives text messages from BLE clients and displays them on an Adafruit SSD1306 OLED display.

## Features

- **BLE Server**: Acts as a Bluetooth Low Energy peripheral device
- **Text Display**: Receives and displays text messages on a 128x64 OLED screen
- **Word Wrapping**: Automatically wraps long messages to fit the display
- **Auto-Reconnect**: Automatically restarts advertising when clients disconnect
- **Serial Monitoring**: Debug output for monitoring connections and messages

## Hardware Requirements

- **ESP32 DOIT DevKit v1** (or compatible ESP32 board)
- **Adafruit SSD1306 OLED Display** (128x64, I2C)
- Jumper wires for connections

## Wiring

Connect the OLED display to the ESP32:

| OLED Pin | ESP32 Pin |
|----------|-----------|
| VCC      | 3.3V      |
| GND      | GND       |
| SDA      | GPIO 21   |
| SCL      | GPIO 22   |

## Software Requirements

- [PlatformIO](https://platformio.org/)
- [Visual Studio Code](https://code.visualstudio.com/) (recommended)

## Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/mastashake08/ESPOLED.git
   cd ESPOLED
   ```

2. Build and upload to your ESP32:
   ```bash
   pio run --target upload
   ```

3. Monitor serial output (optional):
   ```bash
   pio device monitor
   ```

## Usage

### Connecting with a BLE Client

The ESP32 advertises as **"ESP32_OLED"** and can be connected using any BLE client application:

#### Mobile Apps
- **iOS**: LightBlue, nRF Connect
- **Android**: nRF Connect, BLE Scanner

#### Connection Details
- **Device Name**: ESP32_OLED
- **Service UUID**: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`
- **Characteristic UUID**: `beb5483e-36e1-4688-b7f5-ea07361b26a8`

### Sending Messages

1. Connect to the ESP32_OLED device
2. Navigate to the service UUID
3. Write text to the characteristic
4. The message will appear on the OLED display

## Configuration

### Display Settings

Modify in `src/main.cpp`:
```cpp
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C  // I2C address (0x3C or 0x3D)
```

### BLE Device Name

Change the device name in `initBLE()`:
```cpp
BLEDevice::init("ESP32_OLED");  // Change "ESP32_OLED" to your preferred name
```

## Development

### Building
```bash
pio run
```

### Uploading
```bash
pio run --target upload
```

### Clean Build
```bash
pio run --target clean
```

### Serial Monitor
```bash
pio device monitor
```

## Project Structure

```
ESPOLED/
├── src/
│   └── main.cpp          # Main application code
├── include/              # Header files
├── lib/                  # Custom libraries
├── test/                 # Unit tests
├── platformio.ini        # PlatformIO configuration
└── README.md            # This file
```

## Libraries Used

- **Adafruit GFX Library** - Graphics primitives
- **Adafruit SSD1306** - OLED display driver
- **ESP32 BLE Arduino** - Built-in BLE support

## Troubleshooting

### Display Not Working
- Check I2C address (0x3C or 0x3D) - use I2C scanner if unsure
- Verify wiring connections
- Ensure display has power (3.3V or 5V depending on model)

### BLE Not Advertising
- Check serial output for error messages
- Restart the ESP32
- Ensure no other BLE code is conflicting

### Compilation Errors
- Update PlatformIO: `pio upgrade`
- Clean and rebuild: `pio run --target clean && pio run`

## License

This project is open source and available under the MIT License.

## Author

Jyrone Parker ([@mastashake08](https://github.com/mastashake08))

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
