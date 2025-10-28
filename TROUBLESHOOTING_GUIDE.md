# 🔧 Liquid Handling Robot - Troubleshooting Guide

## 🚨 **Common Wiring Issues & Solutions**

### **Problem 1: No outputs working**
**Symptoms:** All LEDs remain off, no activity on any shift register outputs

**Check List:**
- ✅ **Power supply**: 74HC595 Pin 16 = +5V, Pin 8 = GND
- ✅ **Enable pin**: 74HC595 Pin 13 (OE) = GND (active low)
- ✅ **Clear pin**: 74HC595 Pin 10 (SRCLR) = +5V (active low)
- ✅ **ESP32 connections**: GPIO 4, 5, 6 properly connected

**Solutions:**
1. Measure voltages with multimeter
2. Check continuity of all control wires
3. Verify ESP32 pin assignments in code

### **Problem 2: Wrong bit positions active**
**Symptoms:** LED on bit 5 lights when bit 3 should be active

**Likely Causes:**
- ✅ **Bit order confusion**: MSB vs LSB first
- ✅ **Register order**: Wrong daisy chain sequence
- ✅ **Pin mapping**: GPIO assignments incorrect

**Solutions:**
1. Run the walking bit test
2. Create a bit position map
3. Verify daisy chain Q7' → SER connections

### **Problem 3: Intermittent operation**
**Symptoms:** Sometimes works, sometimes doesn't

**Check List:**
- ✅ **Timing**: Clock signals too fast for 74HC595
- ✅ **Power stability**: Voltage drops during operation
- ✅ **Connections**: Loose wires or breadboard contacts
- ✅ **Interference**: Motor noise affecting digital signals

**Solutions:**
1. Add delays between clock pulses
2. Use decoupling capacitors (100nF ceramic + 10µF electrolytic)
3. Twist I2S signal wires together
4. Use shielded cables for long runs

### **Problem 4: Only first register works**
**Symptoms:** Register 0 works, Registers 1 & 2 don't respond

**Check List:**
- ✅ **Daisy chain**: Q7' not connected to next SER
- ✅ **Clock distribution**: SRCLK and RCLK not shared
- ✅ **Power distribution**: Later registers not powered

**Solutions:**
1. Verify Pin 9 (Q7') → Pin 14 (SER) of next register
2. Check all registers share SRCLK and RCLK
3. Measure +5V at each register

## 🔍 **Oscilloscope Verification Points**

### **Signal 1: I2S Data (GPIO 4)**
- **Expected**: Serial data stream during updates
- **Pattern**: 24-bit packets (3 bytes × 8 bits)
- **Timing**: Should change on SRCLK rising edge

### **Signal 2: Shift Clock (GPIO 5)**
- **Expected**: Clock pulses during data transmission
- **Frequency**: ~1-10kHz (adjustable)
- **Duty Cycle**: 50% recommended

### **Signal 3: Latch Clock (GPIO 6)**
- **Expected**: Single pulse after 24 clock cycles
- **Purpose**: Transfer shift register data to outputs
- **Timing**: Rising edge latches data

### **Signal 4: Register Outputs (Q0-Q7)**
- **Expected**: Clean digital high/low levels
- **Voltage**: 0V (low) to +5V (high)
- **Stability**: Should hold state until next update

## 🧪 **Physical Verification Steps**

### **Step 1: Visual Inspection**
1. **Check all solder joints** for cold solder or bridges
2. **Verify IC orientation** - pin 1 indicator correct
3. **Inspect breadboard connections** - no loose wires
4. **Check wire colors** match documentation

### **Step 2: Multimeter Testing**
1. **Continuity test** all control lines
2. **Voltage verification** on power pins
3. **Resistance check** for short circuits
4. **Ground continuity** between all registers

### **Step 3: Logic Analyzer Testing**
1. **Capture I2S signals** during test pattern
2. **Verify bit timing** and order
3. **Check setup/hold times** for reliable operation
4. **Analyze glitches** or signal integrity issues

## 🎯 **Liquid Handling Specific Tests**

### **Syringe Pump Calibration**
```cpp
// Test A-axis movement for liquid handling
void testSyringeCalibration() {
    // Full aspiration (A-axis forward)
    for (int steps = 0; steps < 3200; steps++) {  // 1 full revolution
        setStepperBit(A_STEP, HIGH);
        setStepperBit(A_DIR, HIGH);  // Aspirate direction
        delayMicroseconds(500);      // 1kHz step rate
        
        setStepperBit(A_STEP, LOW);
        delayMicroseconds(500);
    }
    
    delay(1000);
    
    // Full dispense (A-axis reverse)
    for (int steps = 0; steps < 3200; steps++) {
        setStepperBit(A_STEP, HIGH);
        setStepperBit(A_DIR, LOW);   // Dispense direction
        delayMicroseconds(500);
        
        setStepperBit(A_STEP, LOW);
        delayMicroseconds(500);
    }
}
```

### **Positioning Accuracy Test**
```cpp
// Test X,Y,Z positioning accuracy
void testPositioningAccuracy() {
    // Move to known positions and verify with external measurement
    
    // X-axis: 10mm movement test
    moveAxis('X', 2000);  // 2000 steps = 10mm (if 200 steps/mm)
    delay(500);
    // Measure actual position with dial indicator
    
    // Y-axis: 10mm movement test  
    moveAxis('Y', 2000);
    delay(500);
    
    // Z-axis: 5mm movement test
    moveAxis('Z', 2000);  // Different steps/mm for Z
    delay(500);
}
```

## 📊 **Expected Test Results**

### **Verification Test Output:**
```
🤖 LIQUID HANDLING ROBOT - 74HC595 WIRING TEST
================================================

I2S pins initialized:
  Data Pin (SER):   GPIO 4
  Clock Pin (SRCLK): GPIO 5  
  Latch Pin (RCLK):  GPIO 6

📍 Test 1: Individual Bit Verification
Bit  0: Register 0, Bit 0 - X_STEP
Bit  1: Register 0, Bit 1 - X_DIR
Bit  2: Register 0, Bit 2 - Y_STEP
...
Bit 23: Register 2, Bit 7 - EXPANSION_7

✅ All tests completed.
```

### **Success Indicators:**
- ✅ Each bit lights correct LED
- ✅ No crosstalk between channels  
- ✅ Stable output levels
- ✅ Proper sequence timing
- ✅ All 24 bits functional

### **Failure Indicators:**
- ❌ Wrong LEDs lighting up
- ❌ Multiple bits active simultaneously
- ❌ Flickering or unstable outputs
- ❌ Missing bits in sequence
- ❌ No activity on later registers

## 🔄 **Next Steps After Verification**

1. **Upload verification test** to ESP32-S3
2. **Connect LEDs** to all 24 outputs
3. **Run comprehensive test** and document results
4. **Fix any wiring issues** found
5. **Proceed to stepper driver** connections
6. **Test with actual motors** (low current first)
7. **Calibrate liquid handling** volumes and speeds

## 📞 **Support Information**

If you encounter issues not covered here:
1. **Document symptoms** with photos/videos
2. **Capture oscilloscope traces** of problem signals  
3. **Note specific test** that fails
4. **List hardware versions** (ESP32-S3 variant, 74HC595 part numbers)

**Remember**: Always verify with low-current devices (LEDs) before connecting high-power stepper motors!