#pragma once
// clang-format off

/*
    esp32s3_4axis_cnc.h
    
    Custom ESP32-S3 machine configuration for 4-axis CNC with limit switches
    Optimized pin assignments for ESP32-S3-WROOM module
    
    Features:
    - 4-axis stepper motor control (XYZA)
    - 4 limit switches (one per axis)
    - Probe input
    - PWM spindle control
    - Coolant outputs (mist/flood)
    - Emergency stop
    - WiFi enabled (Bluetooth disabled for ESP32-S3)
    
    GPIO Usage Summary:
    - Motors: 8 pins (step/dir for each axis)
    - Limits: 4 pins (X,Y,Z,A limit switches)
    - Control: 6 pins (enable, probe, spindle, coolant, estop)
    - Total used: 18 pins
    - Available: 17+ pins remaining for expansion
    
    Created: 2025 for ESP32-S3 GRBL conversion
    
    Grbl_ESP32 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/

#define MACHINE_NAME            "ESP32-S3 4-Axis CNC Controller"

#ifdef N_AXIS
    #undef N_AXIS
#endif
#define N_AXIS 4

// =====================================================
// STEPPER MOTOR PINS (8 pins total)
// Using GPIO 1-8 for reliable, fast motor control
// =====================================================

#define X_STEP_PIN              GPIO_NUM_1
#define X_DIRECTION_PIN         GPIO_NUM_2

#define Y_STEP_PIN              GPIO_NUM_3
#define Y_DIRECTION_PIN         GPIO_NUM_4

#define Z_STEP_PIN              GPIO_NUM_5
#define Z_DIRECTION_PIN         GPIO_NUM_6

#define A_STEP_PIN              GPIO_NUM_7
#define A_DIRECTION_PIN         GPIO_NUM_8

// Shared stepper enable (active low)
#define STEPPERS_DISABLE_PIN    GPIO_NUM_9

// =====================================================
// LIMIT SWITCHES (4 pins total)
// Using GPIO 10-13 for limit switch inputs
// Configure as INPUT_PULLUP in firmware
// =====================================================

#define X_LIMIT_PIN             GPIO_NUM_10
#define Y_LIMIT_PIN             GPIO_NUM_11
#define Z_LIMIT_PIN             GPIO_NUM_12
#define A_LIMIT_PIN             GPIO_NUM_13

// =====================================================
// PROBE AND SAFETY (2 pins total)
// =====================================================

#define PROBE_PIN               GPIO_NUM_14
#define CONTROL_SAFETY_DOOR_PIN GPIO_NUM_15   // Emergency stop

// =====================================================
// SPINDLE CONTROL (2 pins total)
// PWM spindle with enable control
// =====================================================

#define SPINDLE_TYPE            SpindleType::PWM
#define SPINDLE_OUTPUT_PIN      GPIO_NUM_16   // PWM speed control
#define SPINDLE_ENABLE_PIN      GPIO_NUM_17   // Spindle enable/disable

// =====================================================
// COOLANT CONTROL (2 pins total)
// =====================================================

#define COOLANT_MIST_PIN        GPIO_NUM_18   // Mist coolant
#define COOLANT_FLOOD_PIN       GPIO_NUM_19   // Flood coolant

// =====================================================
// USER I/O AND EXPANSION (Available pins)
// These pins are available for additional features:
// GPIO 20, 21, 35-48 (19 pins available)
// =====================================================

// Optional: Additional control inputs
// #define CONTROL_RESET_PIN       GPIO_NUM_20
// #define CONTROL_FEED_HOLD_PIN   GPIO_NUM_21

// Optional: Status LEDs
// #define STATUS_LED_PIN          GPIO_NUM_35
// #define ALARM_LED_PIN           GPIO_NUM_36

// Optional: Tool change
// #define TOOL_CHANGE_PIN         GPIO_NUM_37

// Optional: Additional spindles or outputs
// #define SPINDLE2_OUTPUT_PIN     GPIO_NUM_38
// #define USER_OUTPUT_1           GPIO_NUM_39
// #define USER_OUTPUT_2           GPIO_NUM_40

// =====================================================
// DEFAULT SETTINGS FOR ESP32-S3 CNC
// =====================================================

// Stepper motor settings (adjust based on your motors)
#define DEFAULT_X_STEPS_PER_MM  80.0    // Steps per mm for X axis
#define DEFAULT_Y_STEPS_PER_MM  80.0    // Steps per mm for Y axis  
#define DEFAULT_Z_STEPS_PER_MM  400.0   // Steps per mm for Z axis
#define DEFAULT_A_STEPS_PER_MM  80.0    // Steps per mm for A axis (rotary)

// Maximum feed rates (mm/min)
#define DEFAULT_X_MAX_RATE      3000.0
#define DEFAULT_Y_MAX_RATE      3000.0
#define DEFAULT_Z_MAX_RATE      1000.0
#define DEFAULT_A_MAX_RATE      1000.0

// Acceleration (mm/sec^2)
#define DEFAULT_X_ACCELERATION  200.0
#define DEFAULT_Y_ACCELERATION  200.0
#define DEFAULT_Z_ACCELERATION  100.0
#define DEFAULT_A_ACCELERATION  100.0

// Homing settings
#define DEFAULT_HOMING_ENABLE   1       // Enable homing
#define DEFAULT_HOMING_DIR_MASK 0       // Move positive during homing (adjust as needed)
#define DEFAULT_HOMING_FEED_RATE 500.0  // Homing seek rate (mm/min)
#define DEFAULT_HOMING_SEEK_RATE 2000.0 // Homing fast rate (mm/min)
#define DEFAULT_HOMING_DEBOUNCE_DELAY 250 // Homing debounce delay (ms)
#define DEFAULT_HOMING_PULLOFF  2.0     // Homing pull-off distance (mm)

// Spindle settings
#define DEFAULT_SPINDLE_RPM_MAX 24000.0 // Maximum spindle RPM
#define DEFAULT_SPINDLE_RPM_MIN 100.0   // Minimum spindle RPM

// Safety settings
#define DEFAULT_SOFT_LIMIT_ENABLE 1     // Enable soft limits
#define DEFAULT_HARD_LIMIT_ENABLE 1     // Enable hard limits

// Work coordinate systems
#define DEFAULT_POSITIONING_MODE 0      // G90 absolute mode

// =====================================================
// ESP32-S3 SPECIFIC OPTIMIZATIONS
// =====================================================

// Disable features not available on ESP32-S3
#ifdef ENABLE_BLUETOOTH
    #undef ENABLE_BLUETOOTH  // Bluetooth Classic not supported
#endif

// Enable WiFi (ESP32-S3 has excellent WiFi performance)
#ifndef ENABLE_WIFI
    #define ENABLE_WIFI
#endif

// Use high-performance timer for step generation
#define STEP_PULSE_DELAY 2              // Microseconds step pulse delay

// =====================================================
// PIN VALIDATION FOR ESP32-S3
// =====================================================

// Compile-time validation for ESP32-S3 compatibility
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    // All pins used (1-19) are valid for ESP32-S3
    #if (X_STEP_PIN > 21 && X_STEP_PIN < 35) || X_STEP_PIN > 48
        #error "X_STEP_PIN not compatible with ESP32-S3"
    #endif
    // Add similar checks for other critical pins if needed
#endif

// =====================================================
// MACHINE SPECIFIC NOTES
// =====================================================

/*
Pin Assignment Summary for ESP32-S3:
=====================================

MOTORS (8 pins):
- X: Step=1, Dir=2
- Y: Step=3, Dir=4  
- Z: Step=5, Dir=6
- A: Step=7, Dir=8
- Enable: 9

LIMITS (4 pins):
- X=10, Y=11, Z=12, A=13

CONTROL (4 pins):
- Probe=14, E-Stop=15
- Spindle PWM=16, Enable=17

COOLANT (2 pins):
- Mist=18, Flood=19

AVAILABLE for expansion:
- GPIO 20, 21, 35-48 (19 pins)

Total used: 18 pins
Total available: 35 pins
Remaining: 17 pins for future expansion
*/

// clang-format on
