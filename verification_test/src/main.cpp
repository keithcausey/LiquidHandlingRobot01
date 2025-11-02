/*
  ESP32-S3 74HC595 Shift Register Test - HARDWARE UART VERSION  
  Test 3x cascaded 74HC595 shift registers for 4-axis liquid handling robot
  Using Serial1 on GPIO 43 (TX), GPIO 44 (RX) - hardware UART pins
  GPIO 4: SER (Serial Data), GPIO 5: SRCLK (Shift Clock), GPIO 6: RCLK (Latch Clock)
*/

#include <Arduino.h>

// 74HC595 shift register control pins
constexpr int SER_PIN = 4;     // Serial data input (GPIO 4)
constexpr int SRCLK_PIN = 5;   // Shift register clock (GPIO 5) 
constexpr int RCLK_PIN = 6;    // Register (latch) clock (GPIO 6)
constexpr int TEST_PIN = 2;    // Visual indicator

void setup() {
    // Use hardware UART instead of USB CDC  
    Serial1.begin(115200, SERIAL_8N1, 44, 43);  // RX=44, TX=43
    delay(1000);  // Simple delay, no USB CDC complications

    Serial1.println("ESP32-S3 74HC595 SHIFT REGISTER TEST");
    Serial1.println("4-Axis Liquid Handling Robot - 3x 74HC595 Control");

    // Configure 74HC595 control pins
    pinMode(SER_PIN, OUTPUT);     // Serial data
    pinMode(SRCLK_PIN, OUTPUT);   // Shift clock
    pinMode(RCLK_PIN, OUTPUT);    // Latch clock
    
    // Initialize all pins LOW
    digitalWrite(SER_PIN, LOW);
    digitalWrite(SRCLK_PIN, LOW);
    digitalWrite(RCLK_PIN, LOW);

    Serial1.println("74HC595 Pin Configuration:");
    Serial1.print("  SER (Serial Data): GPIO");
    Serial1.println(SER_PIN);
    Serial1.print("  SRCLK (Shift Clock): GPIO");
    Serial1.println(SRCLK_PIN);
    Serial1.print("  RCLK (Latch Clock): GPIO");
    Serial1.println(RCLK_PIN);

    // Configure test indicator pin
    pinMode(TEST_PIN, OUTPUT);
    digitalWrite(TEST_PIN, LOW);
    Serial1.print("Test indicator: GPIO");
    Serial1.println(TEST_PIN);

    Serial1.println("Testing 3x cascaded 74HC595 (24 outputs total)");
    Serial1.println("Hardware UART on GPIO 43 (TX), GPIO 44 (RX)");
    Serial1.println("");
}

// Function to shift out data to 74HC595 chain
void shiftOut74HC595(uint32_t data, int numBits) {
    // Shift out MSB first
    for (int i = numBits - 1; i >= 0; i--) {
        // Set data bit
        digitalWrite(SER_PIN, (data >> i) & 0x01);
        
        // Pulse shift clock
        digitalWrite(SRCLK_PIN, HIGH);
        delayMicroseconds(1);  // Small delay for reliable shifting
        digitalWrite(SRCLK_PIN, LOW);
        delayMicroseconds(1);
    }
    
    // Pulse latch clock to update outputs
    digitalWrite(RCLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(RCLK_PIN, LOW);
}

void loop() {
    static unsigned long lastUpdate = 0;
    static unsigned long counter = 0;
    static uint8_t testPattern = 0;
    
    unsigned long currentTime = millis();
    
    // Update shift registers every 2 seconds
    if (currentTime - lastUpdate >= 2000) {
        counter++;
        lastUpdate = currentTime;
        
        // Test different patterns for 3x 74HC595 (24 bits total)
        uint32_t data = 0;
        
        switch (testPattern) {
            case 0: // All OFF
                data = 0x000000;
                Serial1.print("Pattern 0: All OFF (0x000000)");
                break;
                
            case 1: // All ON  
                data = 0xFFFFFF;
                Serial1.print("Pattern 1: All ON (0xFFFFFF)");
                break;
                
            case 2: // Walking 1
                data = 0x000001 << (counter % 24);
                Serial1.print("Pattern 2: Walking 1 (0x");
                Serial1.print(data, HEX);
                Serial1.print(")");
                break;
                
            case 3: // Alternating bits
                data = 0xAAAAAA;  // Binary: 101010...
                Serial1.print("Pattern 3: Alternating (0xAAAAAA)");
                break;
                
            case 4: // Inverse alternating
                data = 0x555555;  // Binary: 010101...
                Serial1.print("Pattern 4: Inv Alternating (0x555555)");
                break;
        }
        
        // Send data to 74HC595 chain
        shiftOut74HC595(data, 24);  // 3 registers × 8 bits = 24 bits
        
        Serial1.print(" | Loop ");
        Serial1.println(counter);
        
        // Cycle through patterns
        if (counter % 5 == 0) {
            testPattern = (testPattern + 1) % 5;
        }
    }
    
    // Fast blink test indicator to show program is running
    digitalWrite(TEST_PIN, HIGH);
    delay(50);
    digitalWrite(TEST_PIN, LOW);
    delay(50);
}

