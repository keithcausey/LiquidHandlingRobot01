# Pin Assignment Corrections Log

## Date: November 14, 2025

## Summary of Corrections

### Limit Switch Pin Assignments

**ERRORS FOUND:**
The limit switch GPIO assignments in `LIQUID_HANDLING_WIRING.md` were incorrect.

**Previous (Incorrect) Assignments:**
- GPIO 10 → X_LIMIT
- GPIO 11 → Y_LIMIT
- GPIO 12 → Z_LIMIT
- GPIO 13 → A_LIMIT

**Corrected Assignments:**
- GPIO 8 → X_LIMIT (X-axis limit switch, Active HIGH)
- GPIO 15 → Y_LIMIT (Y-axis limit switch, Active HIGH)
- GPIO 16 → Z_LIMIT (Z-axis limit switch, Active HIGH)
- GPIO 17 → A_LIMIT (A-axis syringe position sensor, Active HIGH)

### Control Input Pin Reassignments

**Cascading Changes:**
Since GPIOs 15, 16, and 17 were previously assigned to control inputs (EMERGENCY_STOP, RESET, FEED_HOLD), these have been reassigned to available GPIOs:

**Previous Assignments:**
- GPIO 15 → EMERGENCY_STOP
- GPIO 16 → RESET
- GPIO 17 → FEED_HOLD

**New Assignments (Available for Future Use):**
- GPIO 10 → EMERGENCY_STOP (available)
- GPIO 11 → RESET (available)
- GPIO 12 → FEED_HOLD (available)

## Verification Status

✅ **A-Axis Limit Switch (GPIO 17)**: Already tested and verified working
- Confirmed Active HIGH logic
- Successfully used in Safe A-Axis Motor Test program
- Integrated into homing routine with 0.5mm pull-off

⏳ **X, Y, Z Limit Switches**: Not yet physically tested
- GPIO 8 (X_LIMIT): Assigned but not verified
- GPIO 15 (Y_LIMIT): Assigned but not verified
- GPIO 16 (Z_LIMIT): Assigned but not verified

## Impact Assessment

### Affected Documents:
1. ✅ `LIQUID_HANDLING_WIRING.md` - Corrected
2. ⏳ Future GRBL_ESP32 machine configuration files - Will need correct assignments

### Affected Code:
1. ⏳ Future multi-axis test programs - Will use corrected pin assignments
2. ⏳ GRBL_ESP32 custom machine definition - Will require these GPIO assignments

## Testing Requirements

Before proceeding with full 4-axis integration:

1. **X-Axis Limit Switch (GPIO 8)**:
   - Verify Active HIGH logic
   - Test with similar methodology as A-axis
   - Measure soft limit distance from home switch

2. **Y-Axis Limit Switch (GPIO 15)**:
   - Verify Active HIGH logic
   - Test homing sequence
   - Document travel range

3. **Z-Axis Limit Switch (GPIO 16)**:
   - Verify Active HIGH logic
   - Critical for pipette safety (vertical axis)
   - Test pull-off distance for clearance

## Notes

- All limit switches are configured as **Active HIGH** (consistent with A-axis testing)
- 74HC595 shift register outputs remain unchanged (motor STEP/DIR/ENABLE signals)
- GPIO 19 (STEPPER_RESET/Safety output) remains unchanged
- GPIO 21 (feedback monitoring) remains unchanged
- GPIO 48 (NeoPixel) remains unchanged

## Next Steps

1. Update GRBL_ESP32 custom machine definition with correct GPIO assignments
2. Test X, Y, Z limit switches individually using similar test program as A-axis
3. Verify all limit switches before enabling multi-axis homing
4. Update `A_AXIS_TEST_DISCOVERIES.md` to cross-reference this correction log
