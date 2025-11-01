/*
  ESP32-S3 Limit Switch Input Test - HARDWARE UART VERSION
  Test GPIO2 output + 4-axis limit switch inputs for liquid handling robot
  Using Serial1 on GPIO 43 (TX), GPIO 44 (RX) - hardware UART pins
  X, Y, Z positioning axes + A-axis syringe pump
*/

#include <Arduino.h>

// Output test pin
constexpr int TEST_PIN = 2;

// Limit switch input pins for 4-axis liquid handling robot
constexpr int X_LIMIT_PIN = 8;   // X-axis positioning limit (changed from GPIO 7)
constexpr int Y_LIMIT_PIN = 15;  // Y-axis positioning limit  
constexpr int Z_LIMIT_PIN = 16;  // Z-axis positioning limit
constexpr int A_LIMIT_PIN = 17;  // A-axis syringe pump limit

void setup() {
    // Use hardware UART instead of USB CDC
    Serial1.begin(115200, SERIAL_8N1, 44, 43);  // RX=44, TX=43
    delay(1000);  // Simple delay, no USB CDC complications

    Serial1.println("ESP32-S3 HARDWARE UART TEST - WORKING!");
    Serial1.println("4-Axis Liquid Handling Robot - Limit Switch Test");

    // Configure test output pin
    pinMode(TEST_PIN, OUTPUT);
    digitalWrite(TEST_PIN, LOW);
    Serial1.print("Test output pin: GPIO");
    Serial1.println(TEST_PIN);

    // Configure limit switch inputs with pullups
    pinMode(X_LIMIT_PIN, INPUT_PULLUP);
    pinMode(Y_LIMIT_PIN, INPUT_PULLUP);
    pinMode(Z_LIMIT_PIN, INPUT_PULLUP);
    pinMode(A_LIMIT_PIN, INPUT_PULLUP);

    Serial1.println("Limit switch pins configured:");
    Serial1.print("  X-axis: GPIO");
    Serial1.println(X_LIMIT_PIN);
    Serial1.print("  Y-axis: GPIO");
    Serial1.println(Y_LIMIT_PIN);
    Serial1.print("  Z-axis: GPIO");
    Serial1.println(Z_LIMIT_PIN);
    Serial1.print("  A-axis: GPIO");
    Serial1.println(A_LIMIT_PIN);
    Serial1.println("Note: Limit switches active HIGH (3.3V when triggered, LOW when normal)");
    Serial1.println("Hardware UART on GPIO 43 (TX), GPIO 44 (RX)");
}

void loop() {
    static unsigned long counter = 0;
    counter++;

    // Read all limit switches (corrected logic - no inversion needed)
    bool x_limit = digitalRead(X_LIMIT_PIN);   // HIGH = triggered, LOW = normal
    bool y_limit = digitalRead(Y_LIMIT_PIN);
    bool z_limit = digitalRead(Z_LIMIT_PIN);
    bool a_limit = digitalRead(A_LIMIT_PIN);

    // Print status every second
    Serial1.print("Loop ");
    Serial1.print(counter);
    Serial1.print(" | Limits: X=");
    Serial1.print(x_limit ? "HIT" : "---");
    Serial1.print(" Y=");
    Serial1.print(y_limit ? "HIT" : "---");
    Serial1.print(" Z=");
    Serial1.print(z_limit ? "HIT" : "---");
    Serial1.print(" A=");
    Serial1.print(a_limit ? "HIT" : "---");
    Serial1.println("");

    // Toggle test pin
    digitalWrite(TEST_PIN, HIGH);
    delay(300);
    digitalWrite(TEST_PIN, LOW);
    delay(700);
}

