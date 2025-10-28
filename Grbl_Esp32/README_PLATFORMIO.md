# GRBL ESP32 - PlatformIO Project

This project has been converted from an Arduino IDE sketch to a PlatformIO project for better dependency management, build configuration, and development experience.

## Project Structure

```
├── platformio.ini          # Main PlatformIO configuration file
├── src/                     # Source files directory
│   ├── main.cpp            # Main application file (formerly Grbl_Esp32.ino)
│   ├── Grbl.h              # Main GRBL header
│   ├── Config.h            # Configuration settings
│   ├── Motors/             # Motor control modules
│   ├── Spindles/           # Spindle control modules
│   ├── Machines/           # Machine-specific configurations
│   ├── WebUI/              # Web interface modules
│   └── ... (other source files)
├── Custom/                  # Custom machine configurations
├── data/                    # SPIFFS data files (web assets)
├── include/                 # Global header files
├── lib/                     # Project-specific libraries
├── .vscode/                 # VS Code configuration
└── grbl_esp32.code-workspace # VS Code workspace file
```

## Prerequisites

1. **Install PlatformIO Core** (choose one method):
   - **VS Code Extension** (recommended): Install the "PlatformIO IDE" extension in VS Code
   - **Command Line**: Install via pip: `pip install platformio`
   - **Standalone**: Download from https://platformio.org/

2. **VS Code** (recommended IDE):
   - Install VS Code from https://code.visualstudio.com/
   - Install the PlatformIO IDE extension

## Setup Instructions

1. **Open the project**:
   - Open VS Code
   - File → Open Workspace from File → Select `grbl_esp32.code-workspace`
   - Or simply open the project folder in VS Code with PlatformIO extension installed

2. **Install dependencies** (automatic):
   - PlatformIO will automatically download the ESP32 platform and required tools
   - This happens on first build or when running PlatformIO commands

3. **Build the project**:
   - In VS Code: Use the PlatformIO toolbar or Ctrl+Alt+B
   - Command line: `platformio run`

4. **Upload to ESP32**:
   - Connect your ESP32 board via USB
   - In VS Code: Use the PlatformIO upload button or Ctrl+Alt+U
   - Command line: `platformio run --target upload`

## Configuration

### Board Selection
The default configuration is set for `esp32dev`. You can modify this in `platformio.ini`:

```ini
[env:esp32dev]
board = esp32dev  ; Change to your specific board
```

Available boards include:
- `esp32dev` - Generic ESP32 Development Board
- `esp32-wrover-kit` - ESP32-WROVER-KIT
- `nodemcu-32s` - NodeMCU-32S
- `lolin_d32` - LOLIN D32

### Machine Configuration
Configure your specific machine by editing the appropriate files in:
- `src/Machines/` - Pre-configured machine types
- `Custom/` - Custom machine configurations

### Build Options
Modify `platformio.ini` to adjust:
- Build flags and preprocessor definitions
- Upload speed and monitor settings
- Partition schemes
- File system options

## Available Commands

### PlatformIO Core Commands
```bash
# Build project
platformio run

# Upload firmware
platformio run --target upload

# Clean build files
platformio run --target clean

# Serial monitor
platformio device monitor

# Build and upload
platformio run --target upload && platformio device monitor
```

### VS Code Commands (with PlatformIO extension)
- **Build**: Ctrl+Alt+B
- **Upload**: Ctrl+Alt+U
- **Serial Monitor**: Ctrl+Alt+S
- **Clean**: Ctrl+Alt+C

## Troubleshooting

### Common Issues

1. **Build Errors**:
   - Ensure all source files are in the `src/` directory
   - Check that all `#include` paths are correct
   - Verify ESP32 platform is installed: `platformio platform install espressif32`

2. **Upload Errors**:
   - Check USB connection and COM port
   - Hold BOOT button on ESP32 during upload if needed
   - Verify correct board selection in platformio.ini

3. **Library Issues**:
   - ESP32 core libraries are automatically included
   - Add additional libraries to `lib_deps` in platformio.ini if needed

### Getting Help
- PlatformIO Documentation: https://docs.platformio.org/
- GRBL ESP32 Documentation: Check the original project documentation
- ESP32 Arduino Core: https://github.com/espressif/arduino-esp32

## Original Arduino IDE Migration

This project was converted from Arduino IDE with the following changes:
1. Renamed `Grbl_Esp32.ino` to `src/main.cpp`
2. Created `platformio.ini` with ESP32 configuration
3. Adjusted include paths for PlatformIO structure
4. Added VS Code workspace configuration
5. Set up proper directory structure for PlatformIO

The functionality and features remain identical to the original Arduino version.
