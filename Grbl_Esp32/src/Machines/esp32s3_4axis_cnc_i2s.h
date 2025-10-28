#pragma once
// clang-format off

/*
    esp32s3_4axis_cnc_i2s.h
    
    ESP32-S3 4-axis CNC with I2S + 74HC595 shift register control
    High-speed stepper control using I2S peripheral
    
    Features:
    - 4-axis stepper motor control via I2S + 3x 74HC595 shift registers
    - 4 limit switches (GPIO inputs)
    - Probe input
    - PWM spindle control
    - Coolant outputs via shift registers
    - Emergency stop
    - WiFi enabled (Bluetooth disabled for ESP32-S3)
    
    Hardware connections:
    - I2S_DATA (GPIO4) → 74HC595 SER (pin 14)
    - I2S_BCK (GPIO5) → 74HC595 SRCLK (pin 11) 
    - I2S_WS (GPIO6) → 74HC595 RCLK (pin 12)
    - 74HC595 daisy-chained: Q7' → SER of next register
    
    Shift Register Bit Mapping:
    Reg0: X_STEP(0), X_DIR(1), Y_STEP(2), Y_DIR(3), Z_STEP(4), Z_DIR(5), A_STEP(6), A_DIR(7)
    Reg1: SPINDLE_PWM(8), COOLANT_MIST(9), COOLANT_FLOOD(10), SPARE(11-15)
    Reg2: SPARE(16-23) for future expansion
*/

#define MACHINE_NAME "ESP32-S3 4-Axis CNC with I2S Shift Registers"

// Enable I2S stepping
#define USE_I2S_STEPS

// I2S pin base is defined in Pins.h as 128
// I2SO macro is defined in I2SOut.h

#ifdef N_AXIS
#undef N_AXIS
#endif
#define N_AXIS 4

#ifdef HOMING_CYCLE_0
#undef HOMING_CYCLE_0
#endif
#ifdef HOMING_CYCLE_1  
#undef HOMING_CYCLE_1
#endif
#ifdef HOMING_CYCLE_2
#undef HOMING_CYCLE_2  
#endif
#define HOMING_CYCLE_0 bit(Z_AXIS)                // Home Z first
#define HOMING_CYCLE_1 bit(X_AXIS) | bit(Y_AXIS)  // Then home X and Y together
#define HOMING_CYCLE_2 bit(A_AXIS)                // Finally home A axis

// === I2S SHIFT REGISTER CONFIGURATION ===
#define USE_I2S_SHIFT_REGISTERS
#define I2S_DATA_PIN    GPIO_NUM_4   // Serial data to shift registers
#define I2S_BCK_PIN     GPIO_NUM_5   // Bit clock (shift clock SRCLK)
#define I2S_WS_PIN      GPIO_NUM_6   // Word select (latch clock RCLK)

// Motor step/direction pins are handled by I2S shift registers
// Virtual pin numbers for software reference
#define X_STEP_PIN      I2SO(0)  // Shift register bit 0
#define X_DIRECTION_PIN I2SO(1)  // Shift register bit 1
#define Y_STEP_PIN      I2SO(2)  // Shift register bit 2  
#define Y_DIRECTION_PIN I2SO(3)  // Shift register bit 3
#define Z_STEP_PIN      I2SO(4)  // Shift register bit 4
#define Z_DIRECTION_PIN I2SO(5)  // Shift register bit 5
#define A_STEP_PIN      I2SO(6)  // Shift register bit 6
#define A_DIRECTION_PIN I2SO(7)  // Shift register bit 7

// === LIMIT SWITCHES (Direct GPIO) ===
#define X_LIMIT_PIN     GPIO_NUM_10
#define Y_LIMIT_PIN     GPIO_NUM_11  
#define Z_LIMIT_PIN     GPIO_NUM_12
#define A_LIMIT_PIN     GPIO_NUM_13

// === PROBE INPUT ===
#define PROBE_PIN       GPIO_NUM_14

// === CONTROL INPUTS ===
#define CONTROL_SAFETY_DOOR_PIN   GPIO_NUM_15  // Emergency stop
#define CONTROL_RESET_PIN         GPIO_NUM_16  // Reset button
#define CONTROL_FEED_HOLD_PIN     GPIO_NUM_17  // Feed hold
#define CONTROL_CYCLE_START_PIN   GPIO_NUM_18  // Cycle start

// === SPINDLE CONTROL ===
#define SPINDLE_TYPE            SpindleType::PWM
#define SPINDLE_OUTPUT_PIN      GPIO_NUM_21    // PWM output for spindle speed
#define SPINDLE_ENABLE_PIN      GPIO_NUM_NC    // Use PWM pin for enable/disable

// === COOLANT CONTROL (via shift registers) ===
#define COOLANT_MIST_PIN        I2SO(9)   // Shift register bit 9
#define COOLANT_FLOOD_PIN       I2SO(10)  // Shift register bit 10

// === STEPPER MOTOR CONFIGURATION ===
#define STEPPER_RESET_PIN       GPIO_NUM_19  // Common reset for all drivers

// Default stepper settings
#define DEFAULT_STEP_PULSE_MICROSECONDS 2
#define DEFAULT_STEPPER_IDLE_LOCK_TIME 255 // Stay locked

// Homing settings
#define DEFAULT_HOMING_ENABLE 1
#define DEFAULT_HOMING_DIR_MASK 0     // Move positive during homing
#define DEFAULT_HOMING_FEED_RATE 200.0
#define DEFAULT_HOMING_SEEK_RATE 1000.0
#define DEFAULT_HOMING_DEBOUNCE_DELAY 250
#define DEFAULT_HOMING_PULLOFF 3.0

// Axis settings (mm/rev and steps/mm will need calibration)
#define DEFAULT_X_STEPS_PER_MM 200.0
#define DEFAULT_Y_STEPS_PER_MM 200.0  
#define DEFAULT_Z_STEPS_PER_MM 400.0
#define DEFAULT_A_STEPS_PER_MM 200.0

#define DEFAULT_X_MAX_RATE 5000.0
#define DEFAULT_Y_MAX_RATE 5000.0
#define DEFAULT_Z_MAX_RATE 3000.0
#define DEFAULT_A_MAX_RATE 5000.0

#define DEFAULT_X_ACCELERATION 100.0
#define DEFAULT_Y_ACCELERATION 100.0
#define DEFAULT_Z_ACCELERATION 100.0  
#define DEFAULT_A_ACCELERATION 100.0

#define DEFAULT_X_MAX_TRAVEL 300.0
#define DEFAULT_Y_MAX_TRAVEL 300.0
#define DEFAULT_Z_MAX_TRAVEL 100.0
#define DEFAULT_A_MAX_TRAVEL 360.0

// === COMMUNICATION ===
#define ENABLE_WIFI
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// Disable Bluetooth for ESP32-S3
#undef ENABLE_BLUETOOTH

// === USER I/O (Future expansion via shift registers) ===
// Spare shift register bits available: 11-15, 16-23 (13 bits total)
#define USER_DIGITAL_PIN_1      I2SO(11)  // Shift register bit 11
#define USER_DIGITAL_PIN_2      I2SO(12)  // Shift register bit 12
#define USER_DIGITAL_PIN_3      I2SO(13)  // Shift register bit 13
#define USER_DIGITAL_PIN_4      I2SO(14)  // Shift register bit 14

// === NEOPIXEL STATUS LIGHTS ===
#define USER_DIGITAL_PIN_NEOPIXEL   GPIO_NUM_48  // NeoPixel status strip
#define NEOPIXEL_COUNT              8           // Number of LEDs

// === DEBUG AND MONITORING ===
#define DEBUG_STEP_FREQUENCY    // Enable step frequency monitoring
