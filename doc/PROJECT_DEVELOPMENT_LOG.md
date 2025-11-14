# Liquid Handling Robot - Project Development Log

**Project:** 4-Axis Liquid Handling Robot Controller  
**Platform:** ESP32-S3 with GRBL_ESP32 Firmware  
**Repository:** https://github.com/keithcausey/LiquidHandlingRobot01  
**Started:** November 13, 2025  
**Last Updated:** November 14, 2025

---

## Project Overview

### Objective
Build a 4-axis liquid handling robot controller using ESP32-S3 and GRBL_ESP32 firmware, with custom hardware driver architecture using cascaded 74HC595 shift registers.

### System Architecture
- **Controller:** ESP32-S3 DevKit (240MHz, 8MB Flash)
- **Motor Control:** 3× 74HC595 shift registers (24-bit cascade) with 6N136 optoisolators
- **Axes:** X, Y, Z (positioning), A (pipette plunger)
- **Safety:** Hardware emergency stop with AND gate architecture
- **Interface:** WiFi web interface, Serial/Bluetooth control
- **Microstepping:** 16 microsteps/step

---

## Development Timeline

### Phase 1: Hardware Verification (November 13, 2025)

#### Initial Setup
- Downloaded GRBL_ESP32 from GitHub
- Set up PlatformIO development environment
- Created verification test workspace at `verification_test/`
- Configured COM12 (programming) and COM13 (debug UART)

#### 74HC595 Shift Register Testing
**Goal:** Verify 24-bit cascade control for 12 motors (6 axes × 2 motors each)

**Hardware Configuration:**
- GPIO4 → SER (Serial Data)
- GPIO5 → SRCLK (Shift Clock)
- GPIO6 → RCLK (Latch Clock)
- 6N136 Optoisolators with **inverted logic** (software compensation required)

**Key Discovery:**
- Optoisolators connect to LED anodes → signals must be inverted in software
- Implemented `~registerValue` before shifting to 74HC595
- Successfully tested all 24 bits with walking patterns

#### Emergency Stop System Verification
**Architecture:**
```
ESP32 GPIO19 (OUTPUT) ──┐
                        ├── AND Gate ──→ Motor Driver ENABLE
Emergency Stop Switch ──┘         │
                                  └─→ ESP32 GPIO21 (FEEDBACK)
```

**Critical Discovery:**
- Initial error: Reading GPIO19 (OUTPUT) instead of AND gate output
- Solution: Created GPIO21 feedback path from AND gate output
- Verification: GPIO19 HIGH → GPIO21 HIGH ✅, GPIO19 LOW → GPIO21 LOW ✅

### Phase 2: A-Axis Motor Testing (November 13, 2025)

#### Initial Motor Test Development
**Motor Specifications:**
- 200 steps/revolution
- 16 microsteps/step (corrected from initial 256)
- 1mm pitch lead screw
- 3,200 steps/mm resolution
- ~42mm total travel, 41.5mm soft limit measured

**74HC595 Bit Assignment:**
- Bit 6: A_STEP
- Bit 7: A_DIR

#### Debugging Journey

**Issue 1: Character-by-Character Input**
- Problem: PlatformIO monitor sent each keypress immediately
- Impact: Commands like "f100" sent as individual characters
- Solution: Added `send_on_enter` filter to `platformio.ini`
- File: `verification_test/platformio.ini`

**Issue 2: Motor Direction Reversed**
- Problem: `f` (forward) moved toward limit, `b` (backward) moved away
- Expected: `f` = away from limit, `b` = toward limit
- Solution: Swapped boolean values in command processing
- Root cause: Direction bit polarity relative to machine coordinate system

**Issue 3: Wrong Limit Switch Pin**
- Problem: Homing continued past limit switch
- Initial: GPIO13 (incorrect)
- Corrected: GPIO17 (hardware wired to this pin)
- Lesson: Verify hardware connections before debugging code

**Issue 4: Limit Switch Polarity Inverted**
- Problem: Status showed "Limit: HIT" when switch was clear
- Initial logic: Active LOW with `!digitalRead()`
- Corrected: Active HIGH with `digitalRead()`
- Hardware: Pull-up resistor, switch closes to ground when NOT triggered
- When triggered (at home): GPIO17 reads HIGH

**Issue 5: Emergency Stop False Positives**
- Problem: System reported emergency stop active (red LED)
- Root cause: Reading GPIO19 (OUTPUT) as if it were an INPUT
- Solution: Created GPIO21 feedback path from AND gate output
- Verification: Perfect match between GPIO19 output and GPIO21 feedback

#### Motor Specifications Discovery

**Initial Assumptions (Incorrect):**
- 256 microsteps/step
- Travel calculated as 51,200 steps/mm

**Actual Specifications:**
- **16 microsteps/step** (discovered through testing)
- **3,200 steps/mm** (200 steps × 16 / 1mm pitch)
- **0.5mm pull-off:** 1,600 steps
- **41.5mm travel:** ~132,800 steps from home

#### Speed Optimization

**Initial Speed:** 500µs/step (slow for 150,000 step homing range)

**Progressive Speed Increases:**
1. 500µs → 100µs (5× faster)
2. 100µs → 4µs (25× faster) 
3. 4µs → 1µs (3× faster)
4. 1µs → **0µs** (maximum speed - no delay)

**Final Speed Settings:**
- **Homing:** 312µs/step (1 rev/sec) - slow for accuracy
- **Normal:** 0µs/step (maximum speed)
- **Fast:** 31µs/step (10 rev/sec)
- **Calibration:** 100µs/step

**Key Insight:** At 3,200 steps/mm with 16 microstepping, the resolution is so fine that maximum speed is still extremely precise.

#### User Interface Evolution

**Command Format Changes:**

**Initial:** Step-based commands
```
f1000  → Move forward 1000 steps
b500   → Move backward 500 steps
```

**Final:** Millimeter-based commands (human-friendly)
```
f10.05 → Move forward 10.05mm (32,160 steps)
b5.25  → Move backward 5.25mm (16,800 steps)
f42    → Full travel extent
```

**Homing Improvements:**
- Added 0.5mm (1,600 step) pull-off after finding limit
- Increased search range from 5,000 to 150,000 steps (47mm)
- Prevents "homing failed" if motor starts at far end

#### Final A-Axis Test Features

**Implemented Commands:**
- `h/home` - Home to limit switch with 0.5mm pull-off
- `f##.##` - Move forward (away) in mm (0.01-50.00)
- `b##.##` - Move backward (toward) in mm (0.01-50.00)
- `fast` - Set speed to 31µs (10 rev/sec)
- `slow` - Set speed to 0µs (maximum)
- `d#` - Custom delay (50-1000µs)
- `s/status` - Detailed status with mm conversion
- `cal/calibrate` - Auto-calibrate soft limit
- `enable/disable` - Manual motor enable control

**NeoPixel Status Indicators:**
- 🔴 RED - Emergency stop active
- 🟡 YELLOW - Not homed
- 🟢 GREEN - Homed and ready

**Status Display:**
```
📊 Status - Pos: -33600, Limit: clear, E-Stop: OK, Homed: YES
Position: -10.5mm (from home)
```

---

## Phase 3: Version Control & Documentation (November 14, 2025)

### GitHub Repository Setup

**Repository Created:** `LiquidHandlingRobot01`
**Owner:** keithcausey
**URL:** https://github.com/keithcausey/LiquidHandlingRobot01

**Setup Process:**
1. Initialized local git repository
2. Created GitHub repository (with MIT License)
3. Connected local to remote: `git remote add origin`
4. Resolved LICENSE merge conflict
5. Pushed 403 files (1.69 MB) to GitHub

**Git Workflow Established:**
```powershell
# Local work
git add .
git commit -m "description"

# Sync to GitHub
git push

# Get updates
git pull
```

### Documentation Created

**A_AXIS_TEST_DISCOVERIES.md** - Comprehensive technical reference
- Hardware GPIO assignments
- 74HC595 bit mapping
- Motor specifications
- Speed settings
- All debugging discoveries
- Polarity and pin corrections

**PROJECT_DEVELOPMENT_LOG.md** (this document)
- Development timeline
- Decision rationale
- Technical evolution
- Lessons learned

---

## Technical Specifications Summary

### Hardware
- **MCU:** ESP32-S3 DevKit, 240MHz, 8MB Flash
- **Motor Drivers:** 3× 74HC595 (24-bit cascade)
- **Optoisolators:** 6N136 (inverted logic)
- **Communication:** COM12 (upload), COM13 (debug UART @ 115200)
- **Status LED:** NeoPixel on GPIO48

### A-Axis (Tested & Verified)
- **Motor:** 200 steps/rev, 16 microsteps/step
- **Lead Screw:** 1mm pitch
- **Resolution:** 3,200 steps/mm (0.0003125 mm/step)
- **Travel:** 42mm physical, 41.5mm soft limit
- **Limit Switch:** GPIO17, Active HIGH
- **Speed:** 0-312µs/step (max to 1 rev/sec)
- **Homing:** 0.5mm pull-off, 150K step search range

### Safety System
- **Emergency Stop:** Hardware AND gate
- **Control Output:** GPIO19
- **Feedback Input:** GPIO21
- **Logic:** Both HIGH = safe operation

---

## Key Lessons Learned

### Hardware Design
1. **Always verify GPIO assignments** - Don't assume documentation matches hardware
2. **Test polarity first** - Active HIGH vs LOW can waste hours
3. **Feedback loops are essential** - Can't trust OUTPUT pins as INPUT
4. **Optoisolator polarity** - LED anode connections invert signals

### Software Development
1. **PlatformIO filters matter** - `send_on_enter` critical for command input
2. **User-friendly units** - mm-based commands much better than step counts
3. **Generous search ranges** - 5K steps too small, 150K covers full travel
4. **Speed optimization** - Fine microstepping allows maximum speed safely

### Development Process
1. **Incremental testing** - Test each component before integration
2. **Document discoveries immediately** - Easy to forget polarity corrections
3. **Version control from start** - Git invaluable for tracking changes
4. **Comments in code** - Future you will thank present you

---

## Current Status (November 14, 2025)

### ✅ Completed
- [x] ESP32-S3 development environment
- [x] PlatformIO configuration (COM12/13, send_on_enter filter)
- [x] 74HC595 cascade verification (24-bit, optoisolator inversion)
- [x] Emergency stop system (AND gate with feedback)
- [x] A-axis motor control (full testing, all issues resolved)
- [x] Motor specifications determined (16 microsteps, 3200 steps/mm)
- [x] Speed optimization (0µs max speed achieved)
- [x] User interface (mm-based commands, status display)
- [x] GitHub repository setup and synchronization
- [x] Comprehensive documentation

### 🔄 In Progress
- [ ] GRBL_ESP32 machine configuration
- [ ] X, Y, Z axis setup (3 remaining axes)
- [ ] Web interface configuration
- [ ] Full 4-axis integration

### ⏳ Planned
- [ ] Multi-well plate positioning
- [ ] Liquid handling protocols
- [ ] Advanced features (joystick control consideration)
- [ ] Production testing and calibration

---

## Future Considerations

### FluidNC Migration
- **Decision:** Stay with GRBL_ESP32 for initial build
- **Rationale:** 
  - Hardware already tested with GRBL_ESP32 architecture
  - Simpler configuration (C++ machine files vs YAML)
  - Mature, stable codebase
  - Web interface already included
- **Future Option:** Create separate repository for FluidNC version
  - Repo name: `LiquidHandlingRobot02` or `LiquidHandlingRobot01-FluidNC`
  - Allows side-by-side comparison
  - Reference current repo for hardware specs

### Joystick Control
- **Approach:** PC-side control (Option 1)
  - Connect USB gamepad to PC
  - Python/JavaScript script reads joystick
  - Converts to G-code commands
  - Sends via serial/WiFi to GRBL
- **Mapping Concept:**
  - Left stick: X/Y positioning
  - Right stick Y: Z axis (up/down)
  - Trigger/Bumper: A axis (pipette)
  - Buttons: Position presets, speed control
- **Status:** Deferred until basic system operational

---

## Repository Structure

```
LiquidHandlingRobot01/
├── doc/
│   ├── A_AXIS_TEST_DISCOVERIES.md    # Technical reference
│   ├── PROJECT_DEVELOPMENT_LOG.md    # This document
│   ├── 6N137 Series.pdf              # Optoisolator datasheet
│   └── sn74hc595.pdf                 # Shift register datasheet
├── verification_test/
│   ├── src/
│   │   └── main.cpp                  # A-axis motor test
│   └── platformio.ini                # PlatformIO config (COM12/13)
├── Grbl_Esp32/
│   ├── src/                          # GRBL firmware source
│   ├── Custom/                       # Machine definitions
│   └── platformio.ini                # Main GRBL config
├── embedded/
│   └── www/                          # Web interface files
└── LICENSE                           # MIT License

```

---

## Next Steps

### Immediate (Today)
1. Create custom machine definition for 4-axis liquid handler
2. Configure X, Y, Z axes (similar to A-axis process)
3. Test multi-axis coordination
4. Configure web interface
5. Upload to GitHub

### Short Term (This Week)
1. Full system integration testing
2. Emergency stop comprehensive testing
3. Travel limit verification (all axes)
4. Position accuracy testing
5. Documentation updates

### Medium Term (This Month)
1. Liquid handling protocol development
2. Multi-well plate positioning algorithms
3. Volume control calibration
4. Automated testing sequences
5. User manual creation

---

## Contact & Collaboration

**Developer:** Keith Causey  
**Repository:** https://github.com/keithcausey/LiquidHandlingRobot01  
**Platform:** GRBL_ESP32 (https://github.com/bdring/Grbl_Esp32)  
**License:** MIT

---

## Appendix: Command Reference

### A-Axis Test Commands
```
h, home         - Home to limit switch (includes 0.5mm pull-off)
cal, calibrate  - Auto-calibrate soft limit (~35mm travel)
f##.##          - Forward movement in mm (0.01-50.00)
b##.##          - Backward movement in mm (0.01-50.00)
fast            - Set fast speed (31µs, 10 rev/sec)
slow            - Set normal speed (0µs, maximum)
d###            - Set custom delay (50-1000µs)
s, status       - Show detailed status
enable          - Set GPIO19 HIGH (enable motors)
disable         - Set GPIO19 LOW (disable motors)
help, ?         - Show command list
```

### Status Interpretation
```
📊 Status - Pos: -33600, Limit: clear, E-Stop: OK, Homed: YES
             │          │              │            │
             │          │              │            └─ Homing complete
             │          │              └─ Emergency stop not active
             │          └─ Not at limit switch
             └─ -33600 steps = -10.5mm from home
```

---

*Last updated: November 14, 2025*  
*Document version: 1.0*
