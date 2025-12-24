# Copilot Instructions for ESPOLED

## Project Overview

ESP32 BLE server that receives text messages from BLE clients and displays them on an Adafruit OLED screen. Built with PlatformIO and Arduino framework.

**Architecture**: ESP32 acts as BLE peripheral (server) → receives text via BLE characteristic → displays on OLED.

## Platform & Hardware

- **Target Board**: ESP32 DOIT DevKit v1
- **Framework**: Arduino
- **Platform**: Custom ESP32 platform from pioarduino (not official Espressif platform)
- **Build System**: PlatformIO (not Arduino IDE)

## Project Structure

```
src/main.cpp        - Main application code (setup() and loop() functions)
include/            - Project header files (.h)
lib/                - Project-specific libraries (each in own subdirectory)
platformio.ini      - PlatformIO configuration (board, platform, dependencies)
test/               - Unit tests
```

## Development Workflow

### Building & Uploading

Use PlatformIO commands, NOT Arduino IDE:
- Build: `pio run` or `platformio run`
- Upload to device: `pio run --target upload`
- Clean build: `pio run --target clean`
- Monitor serial output: `pio device monitor`

### Adding Libraries

Add dependencies to `platformio.ini` under `[env:esp32doit-devkit-v1]`:
```ini
lib_deps = 
    adafruit/Adafruit GFX Library
    adafruit/Adafruit SSD1306
```

PlatformIO will auto-download libraries on build. Do NOT manually install libraries like in Arduino IDE.

**Required libraries for this project:**
- Adafruit GFX Library (graphics primitives)
- Adafruit SSD1306 or similar (OLED driver - check which Adafruit OLED model)
- ESP32 BLE libraries (built into ESP32 Arduino core, no separate install needed)

## Code Conventions

### Arduino Framework Structure

- `setup()`: Runs once on boot - initialize hardware, serial, peripherals
- `loop()`: Runs continuously - main application logic
- Use `#include <Arduino.h>` at the top (required for PlatformIO Arduino framework)

### Function Organization

- **BLE**: Use `BLEDevice`, `BLEServer`, `BLEService`, `BLECharacteristic` classes from ESP32 BLE library
- **BLE Server Pattern**: Create server → define service → add characteristic → set callbacks for writes
- **OLED I2C**: Default ESP32 I2C pins are GPIO21 (SDA) and GPIO22 (SCL), but can be remapped
- Pin numbers follow ESP32 GPIO numbering, not Arduino board pins
- Watch for flash/RAM constraints - BLE and display both use significant memory
### ESP32-Specific Considerations

- Use ESP32-specific APIs when needed (WiFi, Bluetooth, dual-core, etc.)
- Pin numbers follow ESP32 GPIO numbering, not Arduino board pins
- Watch for flash/RAM constraints on ESP32

## Custom Libraries

Place project-specific libraries in `lib/<LibraryName>/`:
```
lib/MyLibrary/
    MyLibrary.h
    MyLibrary.cpp
```

PlatformIO will automatically link them. No need to modify build configuration.

## Testing

PlaApplication Flow

1. **Setup**: Initialize OLED display → initialize BLE server → create service/characteristic → start advertising
2. **Loop**: BLE handles connections automatically via callbacks
3. **On BLE Write**: Callback receives text → clear/update OLED display → show new message

## Important Notes

- This project uses a custom ESP32 platform URL (pioarduino fork), not the standard Espressif platform
- BLE and OLED must both be initialized in `setup()` before starting BLE advertising
- BLE characteristic callbacks handle incoming messages asynchronously
- OLED display methods are synchronous - call from callback or loop
- This project uses a custom ESP32 platform URL (pioarduino fork), not the standard Espressif platform
- When suggesting code, ensure ESP32 and Arduino framework compatibility
- Serial communication defaults to 115200 baud unless configured otherwise
