/*
  Simple ESP32-S3 Communication Test
  Basic test to verify serial communication is working
*/

#include <Arduino.h>

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(2000);  // Wait for serial port to initialize
  
  // Built-in LED setup
  pinMode(2, OUTPUT);
  
  Serial.println("\n=== ESP32-S3 Simple Communication Test ===");
  Serial.println("Board: ESP32-S3 DevKit");
  Serial.println("Status: RUNNING");
  Serial.println("Test: Basic Serial + LED Blink");
  Serial.println("==========================================\n");
}

void loop() {
  static unsigned long lastTime = 0;
  static int counter = 0;
  
  unsigned long currentTime = millis();
  
  // Print message every 2 seconds
  if (currentTime - lastTime >= 2000) {
    lastTime = currentTime;
    counter++;
    
    Serial.print("Test Message #");
    Serial.print(counter);
    Serial.print(" - Uptime: ");
    Serial.print(currentTime / 1000);
    Serial.println(" seconds");
    
    // Blink built-in LED
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
  }
}