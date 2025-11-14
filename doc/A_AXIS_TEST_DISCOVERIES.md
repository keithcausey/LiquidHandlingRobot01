# A-Axis Motor Test - Discoveries & Configuration
**Date:** November 13, 2025

## A-Axis Motor Specifications

### Mechanical Parameters
- **Travel Extent:** ~42mm (physical measurement)
- **Soft Limit Distance:** 41.5mm (measured from home position)
- **Motor:** 200 steps/revolution
- **Microstepping:** 16 microsteps/step
- **Total Steps per Revolution:** 200 × 16 = 3,200 microsteps/rev
- **Lead Screw Pitch:** 1mm
- **Steps per mm:** 3,200 steps/mm
- **Safe Travel Distance:** ~35mm (for soft limit discovery)

### Speed Settings (finalized)
- **Homing Speed:** 312µs/step (1 rev/sec)
- **Normal Speed:** 0µs/step (maximum - no delay)
- **Fast Speed:** 31µs/step (10 rev/sec)
- **Calibration Speed:** 100µs/step

### Calculated Parameters
- **Microsteps per mm:** To be determined from soft limit calibration
- **Soft Limit Position:** To be discovered (approx 35mm from home)
- **Maximum Safe Position:** Soft limit - safety margin

## Hardware Configuration

### ESP32-S3 GPIO Assignments
- **GPIO4** - SER_PIN (Serial data to 74HC595)
- **GPIO5** - SRCLK_PIN (Shift register clock)
- **GPIO6** - RCLK_PIN (Register clock/latch)
- **GPIO17** - A_LIMIT_PIN (A-axis limit switch) ✅ **ACTIVE HIGH**
- **GPIO19** - EN_OVERRIDE_PIN (Output to AND gate for emergency stop)
- **GPIO21** - EMERGENCY_FEEDBACK_PIN (Input from AND gate output)
- **GPIO48** - NEOPIXEL_PIN (Built-in status LED)

### 74HC595 Bit Assignments
- **Bit 6** - A_STEP (A-axis step signal)
- **Bit 7** - A_DIR (A-axis direction signal)

### Optoisolator Configuration
- **Type:** 6N136 optoisolators
- **Logic:** INVERTED (signals connect to LED anodes, cathodes to ground)
- **Implementation:** Software inverts all 74HC595 outputs before shifting

## Motor Direction Configuration ✅

### Discovered Behavior
- When `forward = true` in code → Motor moves TOWARD limit switch
- When `forward = false` in code → Motor moves AWAY from limit switch

### Command Mapping (Corrected)
- **`f#`** command → `moveSteps(#, false)` → Moves AWAY from limit switch
- **`b#`** command → `moveSteps(#, true)` → Moves TOWARD limit switch
- **Homing** → Uses `forward = true` to move TOWARD limit switch

## Limit Switch Configuration ✅

### Pin Assignment Discovery
- **Initial (incorrect):** GPIO13
- **Corrected:** GPIO17 ✅

### Polarity Discovery
- **Initial (incorrect):** Active LOW with `!digitalRead()`
- **Corrected:** Active HIGH with `digitalRead()` ✅
- **Connection:** Pull-up resistor, switch closes to ground when NOT triggered
- **Logic:** HIGH = limit switch activated (syringe at home position)

## Emergency Stop System ✅

### Hardware Architecture
```
ESP32 GPIO19 (OUTPUT) ──┐
                        ├── AND Gate ──→ Motor Driver ENABLE pins
Emergency Stop Switch ──┘         │
                                  └─→ ESP32 GPIO21 (INPUT - feedback)
```

### GPIO19/GPIO21 Connection Test
- **Test Result:** ✅ Perfect connection verified
- **GPIO19 HIGH** → **GPIO21 reads HIGH** ✅
- **GPIO19 LOW** → **GPIO21 reads LOW** ✅
- **Hardware wiring confirmed working**

### Logic
- **GPIO19 HIGH** = ESP32 enables motors
- **GPIO21 HIGH** = AND gate allows motor operation (safe)
- **GPIO21 LOW** = Emergency stop active (AND gate blocks motors)

## Serial Communication

### PlatformIO Monitor Configuration
- **Baud Rate:** 115200
- **Port:** COM13 (debug UART on GPIO43/44)
- **Critical Filter:** `send_on_enter` ✅
  - Required to send complete commands instead of character-by-character
  - Added to `platformio.ini` monitor_filters

### Commands Available
- **`s`** or **`status`** - Show detailed status
- **`h`** or **`home`** - Home motor to limit switch (pulls off 0.5mm after homing)
- **`cal`** or **`calibrate`** - Auto-calibrate soft limit
- **`f##.##`** - Move forward (away from limit) ##.## mm (0.01-50.00)
- **`b##.##`** - Move backward (toward limit) ##.## mm (0.01-50.00)
- **`d#`** - Set step delay # µs (50-1000)
- **`fast`** - Set fast speed (31µs/step)
- **`slow`** - Set normal speed (0µs/step)
- **`enable`** - Set GPIO19 HIGH (enable motors)
- **`disable`** - Set GPIO19 LOW (disable motors)
- **`help`** or **`?`** - Show command list

## NeoPixel Status Indicators
- **RED** - Emergency stop active
- **YELLOW** - Not homed yet
- **GREEN** - Homed and ready for operation

## Debugging Journey

### Issue 1: Character-by-Character Input
- **Problem:** PlatformIO monitor sent each keypress immediately
- **Solution:** Added `send_on_enter` filter to platformio.ini

### Issue 2: Motor Direction Reversed
- **Problem:** `f` moved toward limit, `b` moved away
- **Solution:** Swapped boolean values in command processing

### Issue 3: Wrong Limit Switch Pin
- **Problem:** Homing continued past limit switch
- **Initial:** GPIO13
- **Solution:** Changed to GPIO17

### Issue 4: Limit Switch Polarity Inverted
- **Problem:** Showed "Limit: HIT" when switch was clear
- **Initial:** Active LOW with `!digitalRead()`
- **Solution:** Changed to Active HIGH with `digitalRead()`

### Issue 5: Emergency Stop False Positives
- **Problem:** Red LED, system thought emergency stop was active
- **Root Cause:** Reading GPIO19 as INPUT when it's an OUTPUT
- **Solution:** Created feedback path - GPIO21 reads AND gate output

## Code Files
- **Main Program:** `verification_test/src/main.cpp`
- **Platform Config:** `verification_test/platformio.ini`
- **Library:** Adafruit NeoPixel @ 1.15.2

## Next Steps for A-Axis Testing
1. ✅ Verify limit switch polarity (upload latest fix)
2. Test homing sequence (`h` command)
3. Measure maximum travel extent
4. Fine-tune step delays for optimal speed
5. Document travel limits for GRBL configuration

## Notes
- Emergency stop AND gate provides hardware safety layer
- Software monitors feedback but cannot override hardware emergency stop
- Position tracking maintains step count from home position
- Maximum travel discovery tracks furthest position reached
