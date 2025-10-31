# ESP32S3 Dual UART Solution - USB CDC Bypass Guide

## Problem Statement
ESP32S3 USB CDC (virtual COM port) often corrupts serial communication in Arduino projects, causing garbled output while boot ROM messages remain clear.

## The Solution: Hardware UART + USB Programming

### Hardware Setup
- **Two USB cables** connected to ESP32S3 DevKit
- **COM12** = Programming port (for uploading firmware)
- **COM13** = Connected to PuTTY for monitoring (but not used for our serial output)
- **GPIO 43** = Hardware UART TX (serial output - connect to USB-to-serial adapter)
- **GPIO 44** = Hardware UART RX (serial input)

### PlatformIO Configuration (platformio.ini)
```ini
[env:esp32s3_test]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200
monitor_port = COM13        ; PuTTY connected but not used for our output
upload_port = COM12         ; Programming via this port
upload_speed = 921600
build_flags = 
    -DCORE_DEBUG_LEVEL=0
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=0    ; CRITICAL: Disable USB CDC
    -DCONFIG_IDF_TARGET_ESP32S3
```

### Arduino Code Configuration
```cpp
void setup() {
    // NEVER use Serial (USB CDC) - use Serial1 (Hardware UART)
    Serial1.begin(115200, SERIAL_8N1, 44, 43);  // RX=44, TX=43
    delay(1000);  // Simple delay, no USB CDC complications
    
    Serial1.println("Hardware UART Working!");
}

void loop() {
    Serial1.println("Loop message via hardware UART");
    delay(1000);
}
```

### Key Points
1. **Use Serial1, NEVER Serial** - Serial uses broken USB CDC
2. **GPIO 43 = TX output** - Connect oscilloscope/USB-to-serial adapter here
3. **GPIO 44 = RX input** - For receiving commands (optional)
4. **Upload via COM12** - Programming port remains USB CDC
5. **115200 baud** - Reliable speed for hardware UART
6. **ARDUINO_USB_CDC_ON_BOOT=0** - Critical setting to disable USB CDC

### Expected Results
- **Programming**: Works normally via COM12 (USB CDC for esptool)
- **Serial output**: Clean, readable text on GPIO 43 at 115200 baud (via USB-to-serial adapter)
- **COM13**: PuTTY connected but shows corrupted USB CDC output (not used)
- **Program output**: Only reliable on hardware UART GPIO 43

### Hardware Connection for Serial Reading
- **USB-to-Serial adapter** or **oscilloscope** to GPIO 43
- **GND connection** between adapter and ESP32S3
- **115200 baud, 8N1** settings in terminal program

### Why This Works
- **Boot ROM**: Uses reliable factory USB protocols for programming
- **Arduino USB CDC**: Often corrupted/timing issues in software stack
- **Hardware UART**: Direct GPIO pins, bulletproof ancient protocol
- **Separation**: Programming and communication use different interfaces

## Success Confirmation
When working correctly, you should see:
1. **Programming output**: "Hash of data verified" success messages
2. **Boot messages**: Clear ESP32-ROM messages on USB CDC
3. **Program output**: Clean serial text on GPIO 43 hardware UART

## Troubleshooting
- If upload fails: Check COM12 port assignment for programming
- If no serial output: Verify GPIO 43 connection and 115200 baud on USB-to-serial adapter
- If garbled text on COM13: This is expected - use GPIO 43 hardware UART instead
- If garbled text on GPIO 43: Ensure ARDUINO_USB_CDC_ON_BOOT=0 is set
- If stuck in boot: Try different GPIO pins, avoid conflicting pins

---
**Generated**: October 30, 2025  
**Status**: Verified working solution  
**Project**: Liquid handling robot ESP32S3 control system