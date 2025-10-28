#pragma once

/*
    I2SOut_ESP32S3.h
    ESP32-S3 I2S Output Driver for Shift Register Control
    
    Supports 3x 74HC595 shift registers (24 outputs total)
    for high-speed stepper motor control
    
    Pin assignments:
    - I2S_DATA (serial data) → 74HC595 SER pin
    - I2S_BCK (bit clock) → 74HC595 SRCLK pin  
    - I2S_WS (word select) → 74HC595 RCLK pin
    
    Data format: 24-bit words (3 bytes per update)
    Bit mapping: [Reg2][Reg1][Reg0] where Reg0 is first in chain
*/

#include "Config.h"

#ifdef CONFIG_IDF_TARGET_ESP32S3

#include <stdint.h>
#include "driver/i2s.h"
#include "driver/gpio.h"

// I2S configuration for shift registers
#define I2S_SHIFT_REG_NUM_REGS      3       // Number of 74HC595 shift registers
#define I2S_SHIFT_REG_TOTAL_BITS    24      // Total output bits (3 x 8)
#define I2S_SHIFT_REG_BYTES_PER_SAMPLE 3   // Bytes per I2S sample

// Default pin assignments (can be overridden in machine config)
#ifndef I2S_DATA_PIN
#define I2S_DATA_PIN    GPIO_NUM_4   // Serial data to shift registers
#endif

#ifndef I2S_BCK_PIN  
#define I2S_BCK_PIN     GPIO_NUM_5   // Bit clock (shift clock)
#endif

#ifndef I2S_WS_PIN
#define I2S_WS_PIN      GPIO_NUM_6   // Word select (latch clock)
#endif

// I2S configuration
#define I2S_SAMPLE_RATE         80000   // 80kHz sample rate for smooth stepping
#define I2S_DMA_BUF_COUNT       4       // Number of DMA buffers
#define I2S_DMA_BUF_LEN         256     // DMA buffer length in samples

// Stepper motor bit assignments in shift registers
// Register 0 (first 8 bits - rightmost in chain)
#define X_STEP_BIT      0
#define X_DIR_BIT       1  
#define Y_STEP_BIT      2
#define Y_DIR_BIT       3
#define Z_STEP_BIT      4
#define Z_DIR_BIT       5
#define A_STEP_BIT      6
#define A_DIR_BIT       7

// Register 1 (middle 8 bits)
#define SPINDLE_PWM_BIT 8   // Future PWM expansion
#define COOLANT_MIST_BIT 9
#define COOLANT_FLOOD_BIT 10
#define SPARE_11        11
#define SPARE_12        12
#define SPARE_13        13
#define SPARE_14        14
#define SPARE_15        15

// Register 2 (last 8 bits - leftmost in chain)  
#define SPARE_16        16
#define SPARE_17        17
#define SPARE_18        18
#define SPARE_19        19
#define SPARE_20        20
#define SPARE_21        21
#define SPARE_22        22
#define SPARE_23        23

// I2S shift register control structure
typedef struct {
    uint32_t current_state;     // Current 24-bit output state
    bool     initialized;       // Initialization flag
    uint32_t step_count;       // Debug: count of steps sent
} i2s_shift_reg_t;

// Function prototypes
int i2s_shift_reg_init(void);
int i2s_shift_reg_deinit(void);
void i2s_shift_reg_write(uint8_t bit_num, bool value);
bool i2s_shift_reg_read(uint8_t bit_num);
void i2s_shift_reg_set_state(uint32_t state);
uint32_t i2s_shift_reg_get_state(void);
void i2s_shift_reg_push_sample(uint32_t state);
void i2s_shift_reg_step_pulse(uint8_t motor_num, bool direction);

// Convenience macros for motor control
#define I2S_STEP_X(dir) i2s_shift_reg_step_pulse(0, dir)
#define I2S_STEP_Y(dir) i2s_shift_reg_step_pulse(1, dir)  
#define I2S_STEP_Z(dir) i2s_shift_reg_step_pulse(2, dir)
#define I2S_STEP_A(dir) i2s_shift_reg_step_pulse(3, dir)

#endif // CONFIG_IDF_TARGET_ESP32S3
