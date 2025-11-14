#pragma once
// clang-format off

/*
    liquid_handling_robot.h
    
    Liquid Handling Robot - 4-Axis System
    ESP32-S3 with I2S + 74HC595 shift register control
    MKS Servo42C stepper motors on all axes
    
    Hardware Configuration:
    - 4-axis: X, Y, Z (positioning), A (syringe pump)
    - All motors: MKS Servo42C, 200 steps/rev, 1mm pitch lead screws
    - Resolution: 200 steps/mm (microstepping currently disabled)
    - I2S control via 3x 74HC595 shift registers (24-bit cascade)
    
    Axis Specifications:
    - X-Axis: Lateral positioning (limit switch GPIO 8)
    - Y-Axis: Lateral positioning (limit switch GPIO 15)
    - Z-Axis: Vertical pipette positioning (limit switch GPIO 16)
    - A-Axis: Syringe pump, 41.5mm travel (limit switch GPIO 17, TESTED)
    
    Safety Features:
    - GPIO19: Stepper reset output to AND gate
    - GPIO21: Safety feedback monitoring from AND gate
    - Limit switches: Active HIGH
    - Emergency stop capability
    
    I2S Shift Register Mapping (inverted logic via 6N136 optoisolators):
    Register 0: X_STEP(0), X_DIR(1), Y_STEP(2), Y_DIR(3), Z_STEP(4), Z_DIR(5), A_STEP(6), A_DIR(7)
    Register 1: X_EN(8), Y_EN(9), Z_EN(10), A_EN(11), USER_PIN_2(12), USER_PIN_3(13), USER_PIN_4(14), SPARE(15)
    Register 2: EXPANSION(16-23) for future use
    
    Date: November 14, 2025
    Repository: github.com/keithcausey/LiquidHandlingRobot01
*/

#define MACHINE_NAME "Liquid Handling Robot (4-Axis)"

// Enable I2S stepping
#define USE_I2S_STEPS
#define USE_I2S_OUT_STREAM

// === AXIS CONFIGURATION ===
#ifdef N_AXIS
#undef N_AXIS
#endif
#define N_AXIS 4

// Homing sequence: Z-axis first (safety), then X/Y together, then A-axis last
#ifdef HOMING_CYCLE_0
#undef HOMING_CYCLE_0
#endif
#ifdef HOMING_CYCLE_1  
#undef HOMING_CYCLE_1
#endif
#ifdef HOMING_CYCLE_2
#undef HOMING_CYCLE_2  
#endif
#define HOMING_CYCLE_0 bit(Z_AXIS)                // Home Z first (vertical safety)
#define HOMING_CYCLE_1 bit(X_AXIS) | bit(Y_AXIS)  // Then home X and Y together
#define HOMING_CYCLE_2 bit(A_AXIS)                // Finally home A axis (syringe)

// === I2S SHIFT REGISTER PINS ===
#define I2S_OUT_BCK     GPIO_NUM_5   // Bit clock → 74HC595 SRCLK (pin 11)
#define I2S_OUT_DATA    GPIO_NUM_4   // Serial data → 74HC595 SER (pin 14)
#define I2S_OUT_WS      GPIO_NUM_6   // Word select/Latch → 74HC595 RCLK (pin 12)

// === MOTOR STEP/DIRECTION PINS (via I2S shift registers) ===
// Note: I2SO() macro maps to shift register bit positions
// Physical output is inverted by 6N136 optoisolators
#define X_STEP_PIN      I2SO(0)  // Register 0, bit 0
#define X_DIRECTION_PIN I2SO(1)  // Register 0, bit 1
#define Y_STEP_PIN      I2SO(2)  // Register 0, bit 2
#define Y_DIRECTION_PIN I2SO(3)  // Register 0, bit 3
#define Z_STEP_PIN      I2SO(4)  // Register 0, bit 4
#define Z_DIRECTION_PIN I2SO(5)  // Register 0, bit 5
#define A_STEP_PIN      I2SO(6)  // Register 0, bit 6
#define A_DIRECTION_PIN I2SO(7)  // Register 0, bit 7

// === MOTOR ENABLE PINS (via I2S shift registers) ===
// MKS Servo42C: ENABLE is Active LOW
#define X_DISABLE_PIN   I2SO(8)   // Register 1, bit 0
#define Y_DISABLE_PIN   I2SO(9)   // Register 1, bit 1
#define Z_DISABLE_PIN   I2SO(10)  // Register 1, bit 2
#define A_DISABLE_PIN   I2SO(11)  // Register 1, bit 3

// === LIMIT SWITCHES (Direct GPIO, Active HIGH) ===
#define X_LIMIT_PIN     GPIO_NUM_8   // X-axis limit switch
#define Y_LIMIT_PIN     GPIO_NUM_15  // Y-axis limit switch
#define Z_LIMIT_PIN     GPIO_NUM_16  // Z-axis limit switch (MOUNTED)
#define A_LIMIT_PIN     GPIO_NUM_17  // A-axis syringe position sensor (TESTED)

// === PROBE INPUT ===
#define PROBE_PIN       GPIO_NUM_14  // Liquid level sensor

// === CONTROL INPUTS (Available for future use) ===
#define CONTROL_SAFETY_DOOR_PIN   GPIO_NUM_10  // Emergency stop (available)
#define CONTROL_RESET_PIN         GPIO_NUM_11  // Reset button (available)
#define CONTROL_FEED_HOLD_PIN     GPIO_NUM_12  // Feed hold (available)
#define CONTROL_CYCLE_START_PIN   GPIO_NUM_18  // Cycle start (available)

// === SAFETY SYSTEM ===
#define STEPPER_RESET_PIN GPIO_NUM_19  // Output to AND gate for all drivers
// GPIO 21 is used for safety feedback monitoring (not defined as GRBL pin)

// === SPINDLE/AUXILIARY CONTROL ===
// Using "spindle" outputs for liquid handling pumps
#define SPINDLE_TYPE            SpindleType::PWM
#define SPINDLE_OUTPUT_PIN      GPIO_NUM_21    // Variable speed pump control
#define SPINDLE_ENABLE_PIN      GPIO_NUM_NC    // Use PWM pin for enable

// === COOLANT/ACCESSORY OUTPUTS (via shift registers) ===
// Repurposed for liquid handling accessories
#define COOLANT_MIST_PIN    I2SO(12)  // USER_PIN_2: Vacuum pump
#define COOLANT_FLOOD_PIN   I2SO(13)  // USER_PIN_3: Waste valve
#define USER_DIGITAL_PIN_0  I2SO(14)  // USER_PIN_4: Sample valve

// === STATUS INDICATOR ===
#define USER_DIGITAL_PIN_1  GPIO_NUM_48  // NeoPixel status LED

// === DEFAULT MOTOR SETTINGS ===
// All motors: MKS Servo42C, 200 steps/rev, 1mm pitch
// Current configuration: Full steps (no microstepping)

#define DEFAULT_X_STEPS_PER_MM 200.0
#define DEFAULT_Y_STEPS_PER_MM 200.0
#define DEFAULT_Z_STEPS_PER_MM 200.0
#define DEFAULT_A_STEPS_PER_MM 200.0

#define DEFAULT_X_MAX_RATE 3000.0     // mm/min (to be tuned)
#define DEFAULT_Y_MAX_RATE 3000.0     // mm/min (to be tuned)
#define DEFAULT_Z_MAX_RATE 2000.0     // mm/min (slower for safety)
#define DEFAULT_A_MAX_RATE 2000.0     // mm/min (syringe pump)

#define DEFAULT_X_ACCELERATION 100.0   // mm/sec^2 (to be tuned)
#define DEFAULT_Y_ACCELERATION 100.0   // mm/sec^2 (to be tuned)
#define DEFAULT_Z_ACCELERATION 50.0    // mm/sec^2 (slower for safety)
#define DEFAULT_A_ACCELERATION 50.0    // mm/sec^2 (syringe pump)

#define DEFAULT_X_MAX_TRAVEL 200.0    // mm (to be measured)
#define DEFAULT_Y_MAX_TRAVEL 200.0    // mm (to be measured)
#define DEFAULT_Z_MAX_TRAVEL 100.0    // mm (to be measured)
#define DEFAULT_A_MAX_TRAVEL 41.5     // mm (VERIFIED: 41.5mm after 0.5mm pull-off)

// === HOMING SETTINGS ===
#define DEFAULT_HOMING_ENABLE true
#define DEFAULT_HOMING_DIR_MASK 0     // All axes home to negative direction initially
#define DEFAULT_HOMING_FEED_RATE 100.0      // mm/min search rate
#define DEFAULT_HOMING_SEEK_RATE 500.0      // mm/min fast rate
#define DEFAULT_HOMING_DEBOUNCE_DELAY 250   // msec
#define DEFAULT_HOMING_PULLOFF 0.5          // mm (A-axis verified at 0.5mm)

// === MACHINE LIMITS ===
#define DEFAULT_SOFT_LIMIT_ENABLE false  // Enable after measuring travel
#define DEFAULT_HARD_LIMIT_ENABLE true   // Hardware limit switches enabled

// === STEPPER CONFIGURATION ===
#define DEFAULT_STEP_PULSE_MICROSECONDS 3
#define DEFAULT_STEPPER_IDLE_LOCK_TIME 255  // Keep motors enabled

// === DIRECTION INVERSION ===
// To be configured based on actual motor wiring
#define DEFAULT_INVERT_ST_ENABLE false  // MKS Servo42C uses Active LOW enable
#define DEFAULT_INVERT_LIMIT_PINS true  // Limit switches are Active HIGH

#define DEFAULT_X_HOMING_MPOS 0.0
#define DEFAULT_Y_HOMING_MPOS 0.0
#define DEFAULT_Z_HOMING_MPOS 0.0
#define DEFAULT_A_HOMING_MPOS 0.0

// === COMMUNICATION ===
#define ENABLE_WIFI
#define ENABLE_BLUETOOTH  // Can be disabled if not needed
#define ENABLE_TELNET
#define ENABLE_HTTP
#define ENABLE_MDNS

// WiFi defaults (can be changed via web interface)
#define DEFAULT_WIFI_MODE WiFiMode::AP  // Start as Access Point
#define DEFAULT_AP_SSID "LiquidHandlingRobot"
#define DEFAULT_AP_PASSWORD "robot123"

// clang-format on
