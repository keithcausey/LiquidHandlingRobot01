/*
  ESP32-S3 74HC595x3 Comprehensive Test - 24-Bit Stepper Motor Patterns
  
  This test demonstrates the full capability of your 4-axis liquid handling robot:
  - 24-bit shift register cascade (3x 74HC595)
  - Stepper motor step/direction patterns
  - Moving bit patterns across all 24 outputs
  - Software inversion for 6N136 optoisolators
  
  HARDWARE CONFIGURATION:
  - ESP32-S3 DevKit
  - 3x 74HC595 shift registers (daisy-chained)
  - 6N136 optoisolators on control lines (inverted logic)
  - LEDs on all 24 outputs for visualization
  
  BIT MAPPING (as designed for 4-axis liquid handling):
  Register 0 (0-7):   X_STEP, X_DIR, Y_STEP, Y_DIR, Z_STEP, Z_DIR, A_STEP, A_DIR
  Register 1 (8-15):  SPINDLE, COOLANT_MIST, COOLANT_FLOOD, USER_1, USER_2, USER_3, USER_4, SPARE
  Register 2 (16-23): All spare bits for future expansion
*/

#include <Arduino.h>

// Function declarations
void runWalkingBitPattern();
void runStepperMotorSimulation(); 
void runRegisterBankTest();
void runLiquidHandlingPattern();
void runChasePattern();
void runAllSystemsTest();

// GPIO pins for 74HC595 control (with 6N136 optoisolator inversion)
constexpr int SER_PIN = 4;      // Serial data to first 74HC595
constexpr int SRCLK_PIN = 5;    // Shift register clock
constexpr int RCLK_PIN = 6;     // Register clock (latch)
constexpr int NEOPIXEL_PIN = 48; // Built-in NeoPixel for status

// 74HC595 configuration
constexpr int NUM_REGISTERS = 3;
constexpr int TOTAL_BITS = 24;

// Global shift register state
uint8_t shiftData[NUM_REGISTERS] = {0, 0, 0};

// NeoPixel control variables
uint32_t neoPixelColor = 0x000000;  // Current color (RGB)
bool neoPixelState = false;

// Simple NeoPixel control functions (bit-banged)
void sendBit(bool bit) {
    if (bit) {
        // Send '1': 800ns high, 450ns low
        digitalWrite(NEOPIXEL_PIN, HIGH);
        delayMicroseconds(1);  // ~800ns (close enough)
        digitalWrite(NEOPIXEL_PIN, LOW);
        // delayMicroseconds(1) is too long for 450ns, so skip
    } else {
        // Send '0': 400ns high, 850ns low  
        digitalWrite(NEOPIXEL_PIN, HIGH);
        // No delay for ~400ns
        digitalWrite(NEOPIXEL_PIN, LOW);
        delayMicroseconds(1);  // ~850ns (close enough)
    }
}

void setNeoPixel(uint8_t red, uint8_t green, uint8_t blue) {
    // NeoPixel expects GRB order
    uint32_t color = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
    
    // Send 24 bits (GRB)
    for (int i = 23; i >= 0; i--) {
        sendBit((color >> i) & 1);
    }
    
    // Reset pulse (>50µs low)
    digitalWrite(NEOPIXEL_PIN, LOW);
    delayMicroseconds(60);
}

void setNeoPixelHSV(uint8_t hue, uint8_t sat, uint8_t val) {
    // Simple HSV to RGB conversion
    uint8_t r, g, b;
    uint8_t region = hue / 43;
    uint8_t remainder = (hue - (region * 43)) * 6;
    
    uint8_t p = (val * (255 - sat)) >> 8;
    uint8_t q = (val * (255 - ((sat * remainder) >> 8))) >> 8;
    uint8_t t = (val * (255 - ((sat * (255 - remainder)) >> 8))) >> 8;
    
    switch (region) {
        case 0: r = val; g = t; b = p; break;
        case 1: r = q; g = val; b = p; break;
        case 2: r = p; g = val; b = t; break;
        case 3: r = p; g = q; b = val; break;
        case 4: r = t; g = p; b = val; break;
        default: r = val; g = p; b = q; break;
    }
    
    setNeoPixel(r, g, b);
}

// Software inversion functions for 6N136 optoisolators
void writeInvertedGPIO(int pin, bool logicalState) {
    digitalWrite(pin, !logicalState);  // Invert for optoisolator
}

void setSER(bool state) { writeInvertedGPIO(SER_PIN, state); }
void setSRCLK(bool state) { writeInvertedGPIO(SRCLK_PIN, state); }
void setRCLK(bool state) { writeInvertedGPIO(RCLK_PIN, state); }

// 74HC595 control functions
void clearAllRegisters() {
    for (int i = 0; i < NUM_REGISTERS; i++) {
        shiftData[i] = 0x00;
    }
}

void updateShiftRegisters() {
    setRCLK(false);  // Prepare for data transmission
    
    // Send data for all registers (MSB first, Register 2 → Register 0)
    for (int reg = NUM_REGISTERS - 1; reg >= 0; reg--) {
        for (int bit = 7; bit >= 0; bit--) {
            bool bitValue = (shiftData[reg] >> bit) & 0x01;
            
            setSER(bitValue);           // Set data bit
            setSRCLK(true);            // Clock high
            delayMicroseconds(1);       // Setup time
            setSRCLK(false);           // Clock low (shift data)
            delayMicroseconds(1);       // Hold time
        }
    }
    
    setRCLK(true);     // Latch data to outputs
    delayMicroseconds(1);
    setRCLK(false);    // Reset latch
}

void setBit(int bitNumber, bool state) {
    if (bitNumber >= 0 && bitNumber < TOTAL_BITS) {
        int reg = bitNumber / 8;
        int bit = bitNumber % 8;
        
        if (state) {
            shiftData[reg] |= (1 << bit);   // Set bit
        } else {
            shiftData[reg] &= ~(1 << bit);  // Clear bit
        }
    }
}

// Stepper motor pattern functions
void stepMotor(int motorNum, bool direction, int steps, int delayMs) {
    int stepBit = motorNum * 2;     // Even bits are step pins
    int dirBit = motorNum * 2 + 1;  // Odd bits are direction pins
    
    setBit(dirBit, direction);  // Set direction
    updateShiftRegisters();
    delay(1);
    
    for (int i = 0; i < steps; i++) {
        setBit(stepBit, true);   // Step high
        updateShiftRegisters();
        delay(delayMs);
        
        setBit(stepBit, false);  // Step low  
        updateShiftRegisters();
        delay(delayMs);
    }
}

void setup() {
    // Initialize hardware UART for debugging (optional - can be removed for clean signals)
    Serial1.begin(115200, SERIAL_8N1, 44, 43);  // RX=44, TX=43
    delay(1000);
    
    // Configure GPIO pins
    pinMode(SER_PIN, OUTPUT);
    pinMode(SRCLK_PIN, OUTPUT);
    pinMode(RCLK_PIN, OUTPUT);
    pinMode(NEOPIXEL_PIN, OUTPUT);
    
    // Initialize shift registers
    clearAllRegisters();
    setSER(false);
    setSRCLK(false);
    setRCLK(false);
    updateShiftRegisters();
    
    // Initialize NeoPixel to green (system ready)
    setNeoPixel(0, 255, 0);  // Bright green
    
    Serial1.println("ESP32-S3 74HC595x3 COMPREHENSIVE TEST");
    Serial1.println("4-Axis Liquid Handling Robot - Full 24-Bit Verification");
    Serial1.println("NeoPixel Status: GREEN = Ready, BLUE = Running, RED = Pattern Change");
    Serial1.println("Ready for pattern demonstrations...");
    Serial1.println("");
    
    delay(2000);  // Show green for 2 seconds
}

void loop() {
    static int testPhase = 0;
    static unsigned long lastUpdate = 0;
    static int patternStep = 0;
    
    unsigned long currentTime = millis();
    
    // Run different test patterns every 1.25 seconds (8x faster)
    if (currentTime - lastUpdate >= 1250) {
        lastUpdate = currentTime;
        patternStep = 0;
        testPhase = (testPhase + 1) % 6;  // 6 different test patterns
        
        // Flash red briefly to indicate pattern change
        setNeoPixel(255, 0, 0);
        delay(200);
        
        clearAllRegisters();
        updateShiftRegisters();
        
        Serial1.print("Starting Test Phase ");
        Serial1.print(testPhase + 1);
        Serial1.print(": ");
        
        // Set NeoPixel color based on test phase
        uint8_t hue = testPhase * 42;  // Different color for each phase
        setNeoPixelHSV(hue, 255, 100);  // Moderate brightness
        
        switch (testPhase) {
            case 0:
                Serial1.println("Individual Bit Walking Pattern");
                break;
            case 1:
                Serial1.println("4-Axis Stepper Motor Simulation");
                break;
            case 2:
                Serial1.println("Register Bank Testing");
                break;
            case 3:
                Serial1.println("Liquid Handling Coordination Pattern");
                break;
            case 4:
                Serial1.println("High-Speed Chase Pattern");
                break;
            case 5:
                Serial1.println("All Systems Test");
                break;
        }
        Serial1.println("");
    }
    
    // Execute current test pattern
    switch (testPhase) {
        case 0:
            runWalkingBitPattern();
            break;
        case 1:
            runStepperMotorSimulation();
            break;
        case 2:
            runRegisterBankTest();
            break;
        case 3:
            runLiquidHandlingPattern();
            break;
        case 4:
            runChasePattern();
            break;
        case 5:
            runAllSystemsTest();
            break;
    }
    
    // NeoPixel breathing effect during operation (8x faster)
    static unsigned long lastBreath = 0;
    if (currentTime - lastBreath >= 6) {  // 6ms updates = very smooth breathing (8x faster)
        lastBreath = currentTime;
        
        uint8_t breathValue = (sin((currentTime % 250) * PI / 125.0) * 127) + 128;  // 8x faster cycle
        uint8_t hue = testPhase * 42;
        setNeoPixelHSV(hue, 255, breathValue / 2);  // Breathing intensity
    }
}

// Test Pattern 1: Walking bit across all 24 outputs (8x faster)
void runWalkingBitPattern() {
    static unsigned long lastStep = 0;
    static int currentBit = 0;
    
    if (millis() - lastStep >= 25) {  // 25ms per bit (8x faster)
        lastStep = millis();
        
        clearAllRegisters();
        setBit(currentBit, true);
        updateShiftRegisters();
        
        currentBit = (currentBit + 1) % TOTAL_BITS;
    }
}

// Test Pattern 2: Simulate 4-axis stepper motor movements (8x faster)
void runStepperMotorSimulation() {
    static unsigned long lastStep = 0;
    static int phase = 0;
    
    if (millis() - lastStep >= 12) {  // 12ms steps (8x faster)
        lastStep = millis();
        
        clearAllRegisters();
        
        switch (phase % 8) {
            case 0: stepMotor(0, true, 1, 6);   break;  // X forward (faster)
            case 1: stepMotor(1, true, 1, 6);   break;  // Y forward  
            case 2: stepMotor(2, true, 1, 6);   break;  // Z forward
            case 3: stepMotor(3, true, 1, 6);   break;  // A (syringe) forward
            case 4: stepMotor(0, false, 1, 6);  break;  // X reverse
            case 5: stepMotor(1, false, 1, 6);  break;  // Y reverse
            case 6: stepMotor(2, false, 1, 6);  break;  // Z reverse  
            case 7: stepMotor(3, false, 1, 6);  break;  // A (syringe) reverse
        }
        
        phase++;
    }
}

// Test Pattern 3: Test each 8-bit register independently (8x faster)
void runRegisterBankTest() {
    static unsigned long lastStep = 0;
    static int currentReg = 0;
    
    if (millis() - lastStep >= 100) {  // 100ms per register (8x faster)
        lastStep = millis();
        
        clearAllRegisters();
        shiftData[currentReg] = 0xFF;  // All bits on in current register
        updateShiftRegisters();
        
        currentReg = (currentReg + 1) % NUM_REGISTERS;
    }
}

// Test Pattern 4: Coordinated liquid handling movements (8x faster)
void runLiquidHandlingPattern() {
    static unsigned long lastStep = 0;
    static int sequence = 0;
    
    if (millis() - lastStep >= 37) {  // 37ms steps (8x faster)
        lastStep = millis();
        
        clearAllRegisters();
        
        // Simulate coordinated X-Y positioning with syringe operation
        switch (sequence % 6) {
            case 0: // Move to pickup position
                setBit(0, true); setBit(2, true);  // X & Y step
                setBit(1, false); setBit(3, true); // X forward, Y forward
                break;
            case 1: // Lower Z axis
                setBit(4, true);  // Z step
                setBit(5, false); // Z down
                break;
            case 2: // Aspirate (A axis)
                setBit(6, true);  // A step  
                setBit(7, true);  // A aspirate direction
                break;
            case 3: // Raise Z axis
                setBit(4, true);  // Z step
                setBit(5, true);  // Z up
                break;
            case 4: // Move to dispense position
                setBit(0, true); setBit(2, true);  // X & Y step
                setBit(1, true); setBit(3, false); // X reverse, Y reverse
                break;
            case 5: // Dispense (A axis)
                setBit(6, true);  // A step
                setBit(7, false); // A dispense direction
                break;
        }
        
        updateShiftRegisters();
        sequence++;
    }
}

// Test Pattern 5: High-speed chase pattern (8x faster)
void runChasePattern() {
    static unsigned long lastStep = 0;
    static int chasePos = 0;
    
    if (millis() - lastStep >= 6) {  // 6ms ultra high speed (8x faster)
        lastStep = millis();
        
        clearAllRegisters();
        
        // 3-bit chase pattern
        for (int i = 0; i < 3; i++) {
            int bitPos = (chasePos + i) % TOTAL_BITS;
            setBit(bitPos, true);
        }
        
        updateShiftRegisters();
        chasePos = (chasePos + 1) % TOTAL_BITS;
    }
}

// Test Pattern 6: All systems comprehensive test (8x faster)
void runAllSystemsTest() {
    static unsigned long lastStep = 0;
    static int pattern = 0;
    
    if (millis() - lastStep >= 18) {  // 18ms updates (8x faster)
        lastStep = millis();
        
        // Complex pattern combining multiple effects
        clearAllRegisters();
        
        uint32_t time_based = (millis() / 12) % 256;  // 8x faster time base
        
        shiftData[0] = time_based;              // Time-based pattern on motors
        shiftData[1] = ~time_based;             // Inverted pattern on auxiliaries  
        shiftData[2] = (time_based >> 1) | (time_based << 7);  // Rotated pattern on spares
        
        updateShiftRegisters();
    }
}

