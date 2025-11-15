/*
  Safe A-Axis Motor Test Program
  
  Motor Specifications:
  - 200 steps/rev × 16 microsteps = 3,200 microsteps/rev
  - Lead screw pitch: 1mm
  - Travel extent: ~42mm physical
  - Safe travel: ~35mm for soft limit discovery
  - Steps/mm: 3,200 steps/mm
  
  Features:
  - Emergency stop monitoring via AND gate feedback (GPIO21)
  - Hard limit switch detection on GPIO17 (active HIGH)
  - Soft limit discovery and calibration
  - Fast movement (10× speed capability)
  - Homing routine with limit switch detection
  - Position tracking and travel extent measurement
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// GPIO pins for 74HC595 control (with 6N136 optoisolator inversion)
constexpr int SER_PIN = 4;      // Serial data
constexpr int SRCLK_PIN = 5;    // Shift register clock
constexpr int RCLK_PIN = 6;     // Register clock (latch)

// Safety pins
constexpr int A_LIMIT_PIN = 17;              // A-axis limit switch
constexpr int EN_OVERRIDE_PIN = 19;          // Output to AND gate
constexpr int EMERGENCY_FEEDBACK_PIN = 21;   // Input from AND gate
constexpr int NEOPIXEL_PIN = 48;             // Status LED

// Debug UART - USING USB SERIAL INSTEAD
// constexpr int DEBUG_UART_RX = 44;
// constexpr int DEBUG_UART_TX = 43;

// Motor bit positions in 74HC595 cascade
constexpr int A_STEP_BIT = 6;
constexpr int A_DIR_BIT = 7;

// Motor specifications
constexpr int STEPS_PER_REV = 200;
constexpr int MICROSTEPS_PER_STEP = 1;
constexpr int MICROSTEPS_PER_REV = STEPS_PER_REV * MICROSTEPS_PER_STEP;  // 200
constexpr float TRAVEL_EXTENT_MM = 42.0;  // Measured physical travel
constexpr float SAFE_TRAVEL_MM = 35.0;    // Safe distance for soft limit discovery

// Speed settings (microseconds delay between steps)
constexpr int HOMING_SPEED = 312;      // 1 rev/sec for homing
constexpr int NORMAL_SPEED = 0;        // Maximum speed - no delay
constexpr int FAST_SPEED = 31;         // 10 rev/sec top speed
constexpr int CALIBRATION_SPEED = 100; // Speed for measuring travel

// Motor state
struct MotorState {
    int32_t currentPosition;    // Steps from home
    bool isHomed;              
    bool limitSwitchActive;    
    bool emergencyStop;        
    int stepDelay;             // Current speed (microseconds)
    int32_t maxTravelFound;    // Furthest position reached
    int32_t softLimit;         // Calculated soft limit position
    bool softLimitSet;         // Has soft limit been calibrated?
    float stepsPerMm;          // Calibrated steps per mm
};

MotorState motor = {0, false, false, false, NORMAL_SPEED, 0, 0, false, 0.0};
uint32_t registerState = 0;
Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// 74HC595 control functions (with optoisolator inversion)
void setRegisterBit(int bit, bool value) {
    if (value) {
        registerState |= (1UL << bit);
    } else {
        registerState &= ~(1UL << bit);
    }
}

void shiftOut24Bits() {
    for (int i = 23; i >= 0; i--) {
        bool bitValue = (registerState >> i) & 1;
        digitalWrite(SER_PIN, !bitValue);  // Invert for optoisolator
        digitalWrite(SRCLK_PIN, HIGH);
        delayMicroseconds(1);
        digitalWrite(SRCLK_PIN, LOW);
    }
    digitalWrite(RCLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(RCLK_PIN, LOW);
}

void clearAllRegisters() {
    registerState = 0;
    shiftOut24Bits();
}

void checkSafety() {
    motor.limitSwitchActive = digitalRead(A_LIMIT_PIN);  // Active HIGH
    motor.emergencyStop = !digitalRead(EMERGENCY_FEEDBACK_PIN);  // LOW = emergency
    
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug >= 3000) {
        lastDebug = millis();
        Serial.print("📊 Status - Pos: ");
        Serial.print(motor.currentPosition);
        Serial.print(", Limit: ");
        Serial.print(motor.limitSwitchActive ? "HIT" : "clear");
        Serial.print(", E-Stop: ");
        Serial.print(motor.emergencyStop ? "ACTIVE" : "OK");
        Serial.print(", Homed: ");
        Serial.println(motor.isHomed ? "YES" : "NO");
    }
}

void updateNeoPixel() {
    if (motor.emergencyStop) {
        pixel.setPixelColor(0, pixel.Color(255, 0, 0));  // Red
    } else if (!motor.isHomed) {
        pixel.setPixelColor(0, pixel.Color(255, 255, 0));  // Yellow
    } else {
        pixel.setPixelColor(0, pixel.Color(0, 255, 0));  // Green
    }
    pixel.show();
}

void singleStep(bool forward) {
    if (motor.emergencyStop) {
        Serial.println("❌ Emergency stop active!");
        return;
    }
    
    // Set direction
    setRegisterBit(A_DIR_BIT, forward);
    shiftOut24Bits();
    delay(1);
    
    // Step pulse
    setRegisterBit(A_STEP_BIT, true);
    shiftOut24Bits();
    delayMicroseconds(10);
    setRegisterBit(A_STEP_BIT, false);
    shiftOut24Bits();
    
    // Update position
    motor.currentPosition += forward ? 1 : -1;
    if (motor.currentPosition > motor.maxTravelFound) {
        motor.maxTravelFound = motor.currentPosition;
    }
}

void moveSteps(int steps, bool forward) {
    Serial.print("Moving ");
    Serial.print(steps);
    Serial.print(forward ? " steps TOWARD limit" : " steps AWAY from limit");
    Serial.print(" @ ");
    Serial.print(motor.stepDelay);
    Serial.println("us/step");
    
    for (int i = 0; i < steps; i++) {
        checkSafety();
        if (motor.emergencyStop) {
            Serial.println("❌ Stopped by emergency stop!");
            break;
        }
        if (motor.limitSwitchActive && forward) {
            Serial.println("⚠️ Hard limit switch hit!");
            break;
        }
        // Check soft limit if calibrated
        if (motor.softLimitSet && !forward && motor.currentPosition >= motor.softLimit) {
            Serial.println("⚠️ Soft limit reached!");
            break;
        }
        
        singleStep(forward);
        delay(motor.stepDelay);
        
        if ((i + 1) % 50 == 0) {
            Serial.print(".");
        }
    }
    
    Serial.println("");
    Serial.print("✅ Complete. Position: ");
    Serial.println(motor.currentPosition);
}

void homeMotor() {
    Serial.println("🏠 Starting homing sequence...");
    
    if (motor.limitSwitchActive) {
        Serial.println("Already at limit switch, moving away first...");
        moveSteps(100, false);  // Move away from limit
        delay(500);
    }
    
    Serial.println("Moving toward limit switch...");
    motor.stepDelay = HOMING_SPEED;
    
    for (int i = 0; i < 150000; i++) {  // 150,000 steps = ~47mm (more than 42mm travel)
        checkSafety();
        if (motor.emergencyStop) {
            Serial.println("❌ Homing aborted - emergency stop!");
            return;
        }
        if (motor.limitSwitchActive) {
            Serial.println("✅ Limit switch found!");
            motor.currentPosition = 0;
            motor.isHomed = true;
            motor.maxTravelFound = 0;
            
            // Pull off 0.5mm from limit switch (good practice for restart)
            Serial.println("Pulling off 0.5mm from limit...");
            int pullOffSteps = 100;  // 0.5mm @ 1 microstep/step (200 steps/rev ÷ 1mm pitch = 200 steps/mm)
            for (int j = 0; j < pullOffSteps; j++) {
                singleStep(false);  // Move away from limit
                delayMicroseconds(HOMING_SPEED);
            }
            motor.currentPosition = -pullOffSteps;  // Update position
            
            motor.stepDelay = NORMAL_SPEED;
            Serial.print("✅ Homing complete! Position: ");
            Serial.print(motor.currentPosition);
            Serial.println(" steps");
            return;
        }
        
        singleStep(true);  // Move toward limit
        delayMicroseconds(HOMING_SPEED);
    }
    
    Serial.println("❌ Homing failed - limit switch not found within 150,000 steps (~47mm)");
    Serial.println("💡 Check wiring or limit switch position");
}

void calibrateSoftLimit() {
    if (!motor.isHomed) {
        Serial.println("❌ Must home first! Use 'h' command.");
        return;
    }
    
    Serial.println("🎯 Starting soft limit calibration...");
    Serial.print("Will travel ~");
    Serial.print(SAFE_TRAVEL_MM);
    Serial.println("mm from home");
    
    // Move away from home at calibration speed
    motor.stepDelay = CALIBRATION_SPEED;
    int32_t startPos = motor.currentPosition;
    
    Serial.println("Moving away from home...");
    for (int i = 0; i < 100000; i++) {  // Large number, will stop at ~35mm
        checkSafety();
        if (motor.emergencyStop) {
            Serial.println("❌ Calibration aborted - emergency stop!");
            return;
        }
        
        singleStep(false);  // Move away from limit
        delayMicroseconds(motor.stepDelay);
        
        // Progress indicator every 1000 steps
        if ((i + 1) % 1000 == 0) {
            Serial.print(".");
        }
    }
    
    Serial.println("");
    int32_t travelSteps = motor.currentPosition - startPos;
    motor.softLimit = motor.currentPosition - 200;  // 200 step safety margin
    motor.softLimitSet = true;
    motor.stepsPerMm = (float)travelSteps / SAFE_TRAVEL_MM;
    
    Serial.println("✅ Calibration complete!");
    Serial.print("Traveled: ");
    Serial.print(travelSteps);
    Serial.println(" steps");
    Serial.print("Steps per mm: ");
    Serial.println(motor.stepsPerMm, 2);
    Serial.print("Soft limit set at: ");
    Serial.print(motor.softLimit);
    Serial.println(" steps");
    
    motor.stepDelay = NORMAL_SPEED;  // Restore normal speed
}

void processCommands() {
    if (!Serial.available()) return;
    
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();
    
    Serial.print("Command: ");
    Serial.println(cmd);
    
    if (cmd == "s" || cmd == "status") {
        Serial.println("═══ A-AXIS STATUS ═══");
        Serial.print("Position: ");
        Serial.print(motor.currentPosition);
        Serial.print(" steps (");
        if (motor.stepsPerMm > 0) {
            Serial.print(motor.currentPosition / motor.stepsPerMm, 2);
            Serial.println(" mm)");
        } else {
            Serial.println("not calibrated)");
        }
        Serial.print("Homed: ");
        Serial.println(motor.isHomed ? "YES" : "NO");
        Serial.print("Soft Limit: ");
        if (motor.softLimitSet) {
            Serial.print(motor.softLimit);
            Serial.println(" steps");
        } else {
            Serial.println("Not calibrated");
        }
        Serial.print("Steps/mm: ");
        if (motor.stepsPerMm > 0) {
            Serial.println(motor.stepsPerMm, 2);
        } else {
            Serial.println("Not calibrated");
        }
        Serial.print("Limit Switch: ");
        Serial.println(motor.limitSwitchActive ? "ACTIVE" : "clear");
        Serial.print("Emergency Stop: ");
        Serial.println(motor.emergencyStop ? "ACTIVE" : "OK");
        Serial.print("Speed: ");
        Serial.print(motor.stepDelay);
        Serial.println(" us/step");
        Serial.print("Max Travel: ");
        Serial.print(motor.maxTravelFound);
        Serial.println(" steps");
        return;
    }
    
    if (motor.emergencyStop && cmd != "enable") {
        Serial.println("❌ Emergency stop active! Send 'enable' to clear.");
        return;
    }
    
    if (cmd == "h" || cmd == "home") {
        homeMotor();
    }
    else if (cmd == "cal" || cmd == "calibrate") {
        calibrateSoftLimit();
    }
    else if (cmd == "slow") {
        motor.stepDelay = NORMAL_SPEED;
        Serial.print("Speed set to NORMAL (");
        Serial.print(NORMAL_SPEED);
        Serial.println(" us/step)");
    }
    else if (cmd == "fast") {
        motor.stepDelay = FAST_SPEED;
        Serial.print("Speed set to FAST (");
        Serial.print(FAST_SPEED);
        Serial.println(" us/step)");
    }
    else if (cmd.startsWith("f")) {
        float mm = cmd.substring(1).toFloat();
        if (mm > 0 && mm <= 50.0) {  // Allow up to 50mm travel
            int steps = (int)(mm * 3200);  // Convert mm to steps (3200 steps/mm)
            moveSteps(steps, false);  // f = away from limit
        } else {
            Serial.println("❌ Invalid (0.01-50.00 mm)");
        }
    }
    else if (cmd.startsWith("b")) {
        float mm = cmd.substring(1).toFloat();
        if (mm > 0 && mm <= 50.0) {  // Allow up to 50mm travel
            int steps = (int)(mm * 3200);  // Convert mm to steps (3200 steps/mm)
            moveSteps(steps, true);  // b = toward limit
        } else {
            Serial.println("❌ Invalid (0.01-50.00 mm)");
        }
    }
    else if (cmd.startsWith("d")) {
        int delay = cmd.substring(1).toInt();
        if (delay >= 50 && delay <= 1000) {
            motor.stepDelay = delay;
            Serial.print("Step delay set to ");
            Serial.print(delay);
            Serial.println(" us");
        } else {
            Serial.println("❌ Invalid delay (50-1000 us)");
        }
    }
    else if (cmd == "enable") {
        digitalWrite(EN_OVERRIDE_PIN, HIGH);
        Serial.println("✅ Motor enable set HIGH");
    }
    else if (cmd == "disable") {
        digitalWrite(EN_OVERRIDE_PIN, LOW);
        Serial.println("⚠️ Motor enable set LOW");
    }
    else if (cmd == "help" || cmd == "?") {
        Serial.println("═══ COMMANDS ═══");
        Serial.println("Motor Specs: 200 steps/rev × 16 microsteps = 3,200 steps/rev");
        Serial.println("Lead screw: 1mm pitch, Travel: ~42mm physical");
        Serial.println("Resolution: 3,200 steps/mm (0.0003125 mm/step)");
        Serial.println("");
        Serial.println("s/status - Show detailed status");
        Serial.println("h/home - Home to limit switch");
        Serial.println("cal/calibrate - Auto-calibrate soft limit (~35mm)");
        Serial.println("fast - Set fast speed (50us/step)");
        Serial.println("slow - Set normal speed (200us/step)");
        Serial.println("f##.## - Forward in mm (0.01-50.00, away from limit)");
        Serial.println("b##.## - Backward in mm (0.01-50.00, toward limit)");
        Serial.println("d# - Set delay # us (50-1000)");
        Serial.println("enable/disable - Control GPIO19");
    }
    else {
        Serial.println("❌ Unknown command. Type 'help' for commands.");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // Configure pins
    pinMode(SER_PIN, OUTPUT);
    pinMode(SRCLK_PIN, OUTPUT);
    pinMode(RCLK_PIN, OUTPUT);
    pinMode(EN_OVERRIDE_PIN, OUTPUT);
    pinMode(A_LIMIT_PIN, INPUT_PULLUP);
    pinMode(EMERGENCY_FEEDBACK_PIN, INPUT_PULLUP);
    
    digitalWrite(EN_OVERRIDE_PIN, HIGH);  // Enable motors
    
    pixel.begin();
    pixel.setBrightness(50);
    pixel.show();
    
    clearAllRegisters();
    
    Serial.println("═══════════════════════════════════");
    Serial.println("  Safe A-Axis Motor Test");
    Serial.println("═══════════════════════════════════");
    Serial.println("Type 'help' for commands");
    Serial.println("Type 'h' to home the motor");
    Serial.println("Type 's' for status");
    Serial.println("");
}

void loop() {
    checkSafety();
    processCommands();
    updateNeoPixel();
    delay(10);
}
