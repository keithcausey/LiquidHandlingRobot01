# 🤖 Liquid Handling Robot - 74HC595 Wiring Guide

## 📋 **System Configuration**
- **X Axis**: Linear rail for pipette X positioning
- **Y Axis**: Linear rail for pipette Y positioning  
- **Z Axis**: Linear rail for pipette Z positioning (vertical)
- **A Axis**: Syringe pump drive for liquid handling

## 🔌 **ESP32-S3 to 74HC595 Connections**

### **Primary I2S Control Lines:**
```
ESP32-S3 Pin    →  74HC595 Pin    →  Description
─────────────────────────────────────────────────
GPIO 4          →  Pin 14 (SER)   →  Serial Data Input
GPIO 5          →  Pin 11 (SRCLK) →  Shift Register Clock
GPIO 6          →  Pin 12 (RCLK)  →  Storage Register Clock (Latch)
```

### **74HC595 Daisy Chain Configuration:**
```
[ESP32-S3] → [74HC595 #1] → [74HC595 #2] → [74HC595 #3]
    │            │              │              │
   GPIO4      Pin 9 (Q7')    Pin 9 (Q7')   Pin 9 (Q7')
              →Pin 14(SER)   →Pin 14(SER)   →Pin 14(SER)
```

### **Power and Control:**
```
Pin 16 (VCC)    →  +5V (isolated supply)
Pin 8 (GND)     →  GND (isolated ground)
Pin 13 (OE)     →  GND (always enabled)
Pin 10 (SRCLR)  →  +5V (never clear)
```

## 🔧 **Shift Register Bit Mapping**

### **Register 0 (Motor Control):**
```
Bit  Pin   Function        Wire Color   Stepper Driver
───  ───   ────────────    ──────────   ─────────────
0    Q0    X_STEP          Red          X-Driver STEP
1    Q1    X_DIRECTION     Orange       X-Driver DIR
2    Q2    Y_STEP          Yellow       Y-Driver STEP
3    Q3    Y_DIRECTION     Green        Y-Driver DIR
4    Q4    Z_STEP          Blue         Z-Driver STEP
5    Q5    Z_DIRECTION     Purple       Z-Driver DIR
6    Q6    A_STEP          Gray         A-Driver STEP
7    Q7    A_DIRECTION     White        A-Driver DIR
```

### **Register 1 (Auxiliary Control):**
```
Bit  Pin   Function        Wire Color   Connection
───  ───   ────────────    ──────────   ──────────
8    Q0    X_ENABLE        Brown        X-Servo ENABLE (Active LOW)
9    Q1    Y_ENABLE        Pink         Y-Servo ENABLE (Active LOW)
10   Q2    Z_ENABLE        Cyan         Z-Servo ENABLE (Active LOW)
11   Q3    A_ENABLE        Black        A-Servo ENABLE (Active LOW)
12   Q4    USER_PIN_2      Silver       Vacuum pump
13   Q5    USER_PIN_3      Gold         Waste valve
14   Q6    USER_PIN_4      Tan          Sample valve
15   Q7    SPARE           Maroon       Future use
```

### **Register 2 (Future Expansion):**
```
Bit  Pin   Function        Available for
───  ───   ────────────    ─────────────
16   Q0    EXPANSION       Additional pumps
17   Q1    EXPANSION       Heating/cooling
18   Q2    EXPANSION       More sensors
19   Q3    EXPANSION       Lighting control
20   Q4    EXPANSION       Barcode scanner
21   Q5    EXPANSION       Camera trigger
22   Q6    EXPANSION       Sample identification
23   Q7    EXPANSION       Error indicators
```

## 📍 **GPIO Pin Assignments (Direct Connections)**

### **Limit Switches:**
```
ESP32-S3 Pin    Function            Connection
────────────    ────────────        ──────────
GPIO 8          X_LIMIT             X-axis limit switch (Active HIGH)
GPIO 15         Y_LIMIT             Y-axis limit switch (Active HIGH)
GPIO 16         Z_LIMIT             Z-axis limit switch (Active HIGH)
GPIO 17         A_LIMIT             A-axis syringe position sensor (Active HIGH)
```

### **Control Inputs:**
```
GPIO 14         PROBE               Liquid level sensor
GPIO 10         EMERGENCY_STOP      Emergency stop button (available)
GPIO 11         RESET               System reset button (available)
GPIO 12         FEED_HOLD           Manual pause button (available)
GPIO 18         CYCLE_START         Start cycle button (available)
```

### **System Control:**
```
GPIO 19         STEPPER_RESET       Common reset to all drivers
GPIO 21         SPINDLE_PWM         Variable speed pump (if needed)
GPIO 48         NEOPIXEL            Status LED strip
```

## 🧪 **Liquid Handling Specific Connections**

### **Syringe Pump (A-Axis) Connections:**
```
A_STEP    →  Stepper driver controlling syringe plunger
A_DIR     →  Forward = aspirate, Reverse = dispense
A_LIMIT   →  Syringe position sensor (full/empty)
```

### **Liquid Handling Accessories:**
```
COOLANT_MIST  →  Wash station pump
COOLANT_FLOOD →  Waste pump
USER_PIN_1    →  Vacuum pump for tip pickup
USER_PIN_2    →  Pressure sensor for blockage detection
USER_PIN_3    →  Waste valve control
USER_PIN_4    →  Sample valve control
```

## 🔍 **Step-by-Step Wiring Verification**

### **Step 1: Power Verification**
1. **Measure voltages** with multimeter:
   - 74HC595 Pin 16 (VCC) = +5V
   - 74HC595 Pin 8 (GND) = 0V
   - Verify isolation from ESP32 power

### **Step 2: I2S Signal Verification**
1. **Connect oscilloscope** to verify signals:
   - GPIO 4 (DATA): Should show serial data stream
   - GPIO 5 (SRCLK): Should show clock pulses
   - GPIO 6 (RCLK): Should show latch pulses

### **Step 3: Shift Register Chain Test**
1. **Test daisy chain continuity**:
   - 74HC595 #1 Pin 9 → 74HC595 #2 Pin 14
   - 74HC595 #2 Pin 9 → 74HC595 #3 Pin 14

### **Step 4: Output Verification**
1. **Test each output** with LED and resistor:
   - Connect LED+resistor between each Q pin and GND
   - Run test pattern to verify correct bit positions