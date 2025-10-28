# Hardware Verification Guide
## ESP32-S3 + 74HC595x3 + Emergency Stop System

### Test Setup Required

#### 1. LED Verification Array
Connect LEDs with 220Ω resistors to verify shift register outputs:

**Shift Register 1 (Motors X, Y, Z):**
- Bits 0-7: X-axis motor signals (STEP, DIR, EN + spares)
- Expected pattern: Motor control sequences

**Shift Register 2 (Motor A + Auxiliary):**
- Bits 8-15: A-axis syringe pump + auxiliary signals
- Expected pattern: Syringe pump control + spare outputs

**Shift Register 3 (System Control):**
- Bits 16-23: System control and expansion
- Expected pattern: Various control signals

#### 2. Emergency Stop Test Setup
- **Emergency Stop Button**: Twist-to-reset mushroom button
- **AND Gate**: 74HC08 connected to GPIO19
- **Test Procedure**: Press E-stop during Phase 6 testing

#### 3. Serial Monitor Connection
After upload, if serial monitor fails:
1. Press RESET button on ESP32-S3
2. Or unplug/reconnect USB cable
3. Retry: `pio device monitor --port COM11 --baud 115200`

### Verification Test Phases

#### Phase 1: Individual Bit Verification
- **Purpose**: Test each of 24 shift register outputs individually
- **Expected**: LEDs light up one at a time, 500ms each
- **Verification**: Count 24 distinct LED activations

#### Phase 2: Shift Register Bank Testing
- **Purpose**: Test each 8-bit register as a unit
- **Expected**: Patterns specific to each register bank
- **Verification**: Distinct patterns for motor control groups

#### Phase 3: Motor Pattern Testing
- **Purpose**: Simulate stepper motor control sequences
- **Expected**: STEP/DIR/EN patterns for each motor
- **Verification**: Proper timing and sequence patterns

#### Phase 4: Liquid Handling Patterns
- **Purpose**: Test syringe pump and positioning coordination
- **Expected**: Complex multi-axis coordination patterns
- **Verification**: Synchronized positioning and pumping actions

#### Phase 5: Walking Bit Pattern
- **Purpose**: Shift single bit across entire 24-bit array
- **Expected**: Single LED "walks" across the array
- **Verification**: Smooth progression from bit 0 to bit 23

#### Phase 6: 🚨 EMERGENCY STOP SYSTEM TESTING 🚨
- **Purpose**: CRITICAL SAFETY VERIFICATION
- **Test Actions**:
  1. Normal operation verification
  2. Emergency stop button press simulation
  3. System disable verification
  4. Reset sequence testing
- **Expected**: Immediate cessation of all motor enables
- **CRITICAL**: All motor enable signals must go LOW instantly

#### Phase 7: Servo Enable Control Verification
- **Purpose**: Test MKS SERVO42C enable control strategy
- **Expected**: Proper enable/disable sequences
- **Verification**: EN signals controlled via emergency stop system

### Troubleshooting Common Issues

#### Serial Monitor Access Issues
```powershell
# Check available ports
pio device list

# Verify ESP32-S3 is on COM11
# Look for Hardware ID: USB VID:PID=303A:1001

# If port is locked, try:
# 1. Press RESET button on ESP32-S3
# 2. Unplug/reconnect USB cable
# 3. Retry serial monitor
```

#### LED Connection Verification
- **Resistors**: 220Ω current limiting resistors required
- **Polarity**: Anode to shift register output, cathode to ground via resistor
- **Voltage**: 3.3V logic levels from ESP32-S3

#### Emergency Stop System Issues
- **Button Wiring**: Check twist-to-reset mushroom button connections
- **AND Gate**: Verify 74HC08 wiring and power supply
- **Pull-down**: Ensure 10kΩ pull-down resistor on GPIO19

### Safety Reminders

⚠️ **CRITICAL SAFETY NOTES**:
1. Emergency stop system is SAFETY-CRITICAL
2. Phase 6 testing verifies life-safety functionality
3. Any emergency stop failure must be resolved before motor connection
4. Laboratory safety standards require proper emergency stop operation

### Success Criteria

#### Test Pass Requirements:
- ✅ All 24 LEDs respond correctly in Phase 1
- ✅ Register banks show distinct patterns in Phase 2
- ✅ Motor patterns are proper STEP/DIR/EN sequences
- ✅ Liquid handling shows coordination patterns
- ✅ Walking bit pattern is smooth and complete
- ✅ **CRITICAL**: Emergency stop disables ALL enables instantly
- ✅ Servo enable control follows safety protocols

#### Ready for Next Phase:
After successful verification, proceed to:
1. MKS SERVO42C stepper servo connections
2. Liquid handling system calibration
3. GRBL CNC firmware integration
4. Laboratory automation deployment

---
**Document**: HARDWARE_VERIFICATION_GUIDE.md  
**Version**: 1.0  
**Project**: ESP32-S3 Liquid Handling Robot Controller  
**Safety Level**: CRITICAL - Emergency Stop System  