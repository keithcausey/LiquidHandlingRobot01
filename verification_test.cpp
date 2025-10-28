/*
  Liquid Handling Robot - 74HC595 Connection Verification Test
  
  This test program helps verify the wiring of your 3x 74HC595 shift register
  chain for the liquid handling robot. It systematically tests each output
  to ensure proper connections.
  
  Hardware Requirements:
  - ESP32-S3
  - 3x 74HC595 shift registers (daisy-chained)
  - LEDs + resistors for visual verification
  - Oscilloscope (optional, for signal verification)
  
  Usage:
  1. Connect LEDs to each shift register output
  2. Upload this test program
  3. Monitor serial output for test results
  4. Verify LEDs light up in correct sequence
*/

#include <Arduino.h>

// I2S pins for shift register control
#define I2S_DATA_PIN    4   // Serial data to first 74HC595
#define I2S_BCK_PIN     5   // Shift clock (SRCLK) 
#define I2S_WS_PIN      6   // Latch clock (RCLK)

// Test configuration
#define NUM_SHIFT_REGISTERS 3
#define TOTAL_BITS (NUM_SHIFT_REGISTERS * 8)  // 24 bits total

// Shift register data buffer
uint8_t shiftData[NUM_SHIFT_REGISTERS];

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("================================================");
    Serial.println("🤖 LIQUID HANDLING ROBOT - 74HC595 WIRING TEST");
    Serial.println("================================================");
    Serial.println();
    
    // Initialize I2S pins
    pinMode(I2S_DATA_PIN, OUTPUT);
    pinMode(I2S_BCK_PIN, OUTPUT);
    pinMode(I2S_WS_PIN, OUTPUT);
    
    // Initialize all outputs low
    digitalWrite(I2S_DATA_PIN, LOW);
    digitalWrite(I2S_BCK_PIN, LOW);
    digitalWrite(I2S_WS_PIN, LOW);
    
    Serial.println("I2S pins initialized:");
    Serial.printf("  Data Pin (SER):   GPIO %d\n", I2S_DATA_PIN);
    Serial.printf("  Clock Pin (SRCLK): GPIO %d\n", I2S_BCK_PIN);
    Serial.printf("  Latch Pin (RCLK):  GPIO %d\n", I2S_WS_PIN);
    Serial.println();
    
    // Clear all shift registers
    clearAllRegisters();
    Serial.println("All shift registers cleared.");
    Serial.println();
    
    delay(1000);
}

void loop() {
    Serial.println("🔍 Starting comprehensive wiring verification...");
    Serial.println();
    
    // Test 1: Individual bit test
    testIndividualBits();
    delay(2000);
    
    // Test 2: Register-by-register test
    testByRegister();
    delay(2000);
    
    // Test 3: Motor control patterns
    testMotorPatterns();
    delay(2000);
    
    // Test 4: Liquid handling patterns
    testLiquidHandlingPatterns();
    delay(2000);
    
    // Test 5: Walking bit pattern
    testWalkingBit();
    delay(5000);
    
    Serial.println("✅ All tests completed. Repeating in 10 seconds...");
    Serial.println("================================================");
    Serial.println();
    delay(10000);
}

void clearAllRegisters() {
    for (int i = 0; i < NUM_SHIFT_REGISTERS; i++) {
        shiftData[i] = 0x00;
    }
    updateShiftRegisters();
}

void updateShiftRegisters() {
    // Latch pin low during data transmission
    digitalWrite(I2S_WS_PIN, LOW);
    delayMicroseconds(1);
    
    // Send data for all registers (MSB first, Register 2 → 1 → 0)
    for (int reg = NUM_SHIFT_REGISTERS - 1; reg >= 0; reg--) {
        shiftOut(I2S_DATA_PIN, I2S_BCK_PIN, MSBFIRST, shiftData[reg]);
    }
    
    // Latch the data
    delayMicroseconds(1);
    digitalWrite(I2S_WS_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(I2S_WS_PIN, LOW);
}

void testIndividualBits() {
    Serial.println("📍 Test 1: Individual Bit Verification");
    Serial.println("   Each bit will be turned on individually");
    Serial.println("   Verify the correct LED lights up for each bit");
    Serial.println();
    
    for (int bit = 0; bit < TOTAL_BITS; bit++) {
        clearAllRegisters();
        
        int reg = bit / 8;
        int regBit = bit % 8;
        
        shiftData[reg] = (1 << regBit);
        updateShiftRegisters();
        
        // Print bit information
        Serial.printf("Bit %2d: Register %d, Bit %d - ", bit, reg, regBit);
        printBitFunction(bit);
        
        delay(500);
    }
    
    clearAllRegisters();
    Serial.println("Individual bit test completed.\n");
}

void testByRegister() {
    Serial.println("📍 Test 2: Register-by-Register Test");
    Serial.println("   Each register will be fully activated");
    Serial.println();
    
    for (int reg = 0; reg < NUM_SHIFT_REGISTERS; reg++) {
        clearAllRegisters();
        shiftData[reg] = 0xFF;  // All bits on
        updateShiftRegisters();
        
        Serial.printf("Register %d: All outputs HIGH\n", reg);
        printRegisterFunction(reg);
        Serial.println();
        
        delay(1000);
    }
    
    clearAllRegisters();
    Serial.println("Register test completed.\n");
}

void testMotorPatterns() {
    Serial.println("📍 Test 3: Stepper Motor Patterns");
    Serial.println("   Testing step/direction patterns for each axis");
    Serial.println();
    
    // Test each axis
    const char* axisNames[] = {"X", "Y", "Z", "A"};
    
    for (int axis = 0; axis < 4; axis++) {
        Serial.printf("Testing %s-axis stepper control:\n", axisNames[axis]);
        
        clearAllRegisters();
        
        // Step pulse pattern
        int stepBit = axis * 2;      // 0, 2, 4, 6
        int dirBit = axis * 2 + 1;   // 1, 3, 5, 7
        
        // Direction forward, step pulse
        shiftData[0] = (1 << dirBit) | (1 << stepBit);
        updateShiftRegisters();
        Serial.printf("  %s_DIR=1, %s_STEP=1 (Forward step)\n", axisNames[axis], axisNames[axis]);
        delay(250);
        
        // Direction forward, no step
        shiftData[0] = (1 << dirBit);
        updateShiftRegisters();
        Serial.printf("  %s_DIR=1, %s_STEP=0 (Forward direction)\n", axisNames[axis], axisNames[axis]);
        delay(250);
        
        // Direction reverse, step pulse
        shiftData[0] = (1 << stepBit);
        updateShiftRegisters();
        Serial.printf("  %s_DIR=0, %s_STEP=1 (Reverse step)\n", axisNames[axis], axisNames[axis]);
        delay(250);
        
        clearAllRegisters();
        delay(500);
    }
    
    Serial.println("Motor pattern test completed.\n");
}

void testLiquidHandlingPatterns() {
    Serial.println("📍 Test 4: Liquid Handling Control Patterns");
    Serial.println("   Testing auxiliary controls for liquid handling");
    Serial.println();
    
    // Test syringe pump (A-axis)
    Serial.println("Syringe Pump Simulation:");
    simulateSyringeOperation();
    
    delay(1000);
    
    // Test auxiliary controls (Register 1)
    Serial.println("Auxiliary Controls:");
    testAuxiliaryControls();
    
    Serial.println("Liquid handling pattern test completed.\n");
}

void simulateSyringeOperation() {
    // Aspirate operation
    Serial.println("  🔵 Aspirating liquid...");
    for (int i = 0; i < 10; i++) {
        clearAllRegisters();
        shiftData[0] = 0x80;  // A_DIR = 1 (aspirate direction)
        updateShiftRegisters();
        delay(50);
        
        shiftData[0] = 0x80 | 0x40;  // A_DIR = 1, A_STEP = 1
        updateShiftRegisters();
        delay(50);
    }
    
    delay(500);
    
    // Dispense operation
    Serial.println("  🔴 Dispensing liquid...");
    for (int i = 0; i < 10; i++) {
        clearAllRegisters();
        shiftData[0] = 0x00;  // A_DIR = 0 (dispense direction)
        updateShiftRegisters();
        delay(50);
        
        shiftData[0] = 0x40;  // A_DIR = 0, A_STEP = 1
        updateShiftRegisters();
        delay(50);
    }
    
    clearAllRegisters();
}

void testAuxiliaryControls() {
    const char* auxNames[] = {
        "SPARE", "COOLANT_MIST", "COOLANT_FLOOD", "USER_PIN_1",
        "USER_PIN_2", "USER_PIN_3", "USER_PIN_4", "SPARE"
    };
    
    for (int bit = 0; bit < 8; bit++) {
        clearAllRegisters();
        shiftData[1] = (1 << bit);
        updateShiftRegisters();
        
        Serial.printf("  Register 1, Bit %d: %s\n", bit, auxNames[bit]);
        delay(300);
    }
    
    clearAllRegisters();
}

void testWalkingBit() {
    Serial.println("📍 Test 5: Walking Bit Pattern");
    Serial.println("   A single bit walks through all 24 positions");
    Serial.println("   Verify LEDs light up in sequence 0→23");
    Serial.println();
    
    for (int bit = 0; bit < TOTAL_BITS; bit++) {
        clearAllRegisters();
        
        int reg = bit / 8;
        int regBit = bit % 8;
        
        shiftData[reg] = (1 << regBit);
        updateShiftRegisters();
        
        Serial.printf("Walking bit: Position %2d ", bit);
        printBitFunction(bit);
        
        delay(200);
    }
    
    clearAllRegisters();
    Serial.println("Walking bit test completed.\n");
}

void printBitFunction(int bit) {
    const char* functions[] = {
        "X_STEP", "X_DIR", "Y_STEP", "Y_DIR", 
        "Z_STEP", "Z_DIR", "A_STEP", "A_DIR",
        "SPARE", "COOLANT_MIST", "COOLANT_FLOOD", "USER_PIN_1",
        "USER_PIN_2", "USER_PIN_3", "USER_PIN_4", "SPARE",
        "EXPANSION_0", "EXPANSION_1", "EXPANSION_2", "EXPANSION_3",
        "EXPANSION_4", "EXPANSION_5", "EXPANSION_6", "EXPANSION_7"
    };
    
    if (bit < 24) {
        Serial.println(functions[bit]);
    } else {
        Serial.println("UNKNOWN");
    }
}

void printRegisterFunction(int reg) {
    switch (reg) {
        case 0:
            Serial.println("  Function: Stepper motor control (X,Y,Z,A step/direction)");
            break;
        case 1:
            Serial.println("  Function: Auxiliary controls (pumps, valves, sensors)");
            break;
        case 2:
            Serial.println("  Function: Future expansion (additional devices)");
            break;
        default:
            Serial.println("  Function: Unknown register");
            break;
    }
}