/*
    I2SOut_ESP32S3.cpp
    ESP32-S3 I2S Output Driver for Shift Register Control
    
    Implements high-speed stepper control using I2S peripheral
    to drive 3x 74HC595 shift registers
*/

#include "I2SOut_ESP32S3.h"

#ifdef CONFIG_IDF_TARGET_ESP32S3

#include "Grbl.h"
#include "Report.h"

// Global I2S shift register control structure
static i2s_shift_reg_t i2s_sr = {0};

/*
 * Initialize I2S peripheral for shift register control
 */
int i2s_shift_reg_init(void) {
    if (i2s_sr.initialized) {
        grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Info, "I2S shift register already initialized");
        return 0;
    }

    // I2S configuration
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = I2S_DMA_BUF_COUNT,
        .dma_buf_len = I2S_DMA_BUF_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    // I2S pin configuration
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_PIN,      // Bit clock → SRCLK
        .ws_io_num = I2S_WS_PIN,        // Word select → RCLK
        .data_out_num = I2S_DATA_PIN,   // Serial data → SER
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    // Install and start I2S driver
    esp_err_t ret = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (ret != ESP_OK) {
        grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Error, "I2S driver install failed: %s", esp_err_to_name(ret));
        return -1;
    }

    ret = i2s_set_pin(I2S_NUM_0, &pin_config);
    if (ret != ESP_OK) {
        grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Error, "I2S pin config failed: %s", esp_err_to_name(ret));
        i2s_driver_uninstall(I2S_NUM_0);
        return -1;
    }

    // Start I2S
    ret = i2s_start(I2S_NUM_0);
    if (ret != ESP_OK) {
        grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Error, "I2S start failed: %s", esp_err_to_name(ret));
        i2s_driver_uninstall(I2S_NUM_0);
        return -1;
    }

    // Initialize state
    i2s_sr.current_state = 0;
    i2s_sr.step_count = 0;
    i2s_sr.initialized = true;

    // Send initial state (all outputs low)
    i2s_shift_reg_set_state(0);

    grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Info, "I2S shift register initialized - 24 outputs available");
    grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Info, "Pins: DATA=%d, BCK=%d, WS=%d", I2S_DATA_PIN, I2S_BCK_PIN, I2S_WS_PIN);
    
    return 0;
}

/*
 * Deinitialize I2S peripheral
 */
int i2s_shift_reg_deinit(void) {
    if (!i2s_sr.initialized) {
        return 0;
    }

    i2s_stop(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_0);
    
    i2s_sr.initialized = false;
    grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Info, "I2S shift register deinitialized");
    
    return 0;
}

/*
 * Write a single bit in the shift register chain
 */
void i2s_shift_reg_write(uint8_t bit_num, bool value) {
    if (!i2s_sr.initialized || bit_num >= I2S_SHIFT_REG_TOTAL_BITS) {
        return;
    }

    if (value) {
        i2s_sr.current_state |= (1UL << bit_num);
    } else {
        i2s_sr.current_state &= ~(1UL << bit_num);
    }
    
    i2s_shift_reg_push_sample(i2s_sr.current_state);
}

/*
 * Read current state of a bit
 */
bool i2s_shift_reg_read(uint8_t bit_num) {
    if (bit_num >= I2S_SHIFT_REG_TOTAL_BITS) {
        return false;
    }
    
    return (i2s_sr.current_state & (1UL << bit_num)) != 0;
}

/*
 * Set the entire 24-bit state at once
 */
void i2s_shift_reg_set_state(uint32_t state) {
    if (!i2s_sr.initialized) {
        return;
    }

    i2s_sr.current_state = state & 0xFFFFFF; // Mask to 24 bits
    i2s_shift_reg_push_sample(i2s_sr.current_state);
}

/*
 * Get current 24-bit state
 */
uint32_t i2s_shift_reg_get_state(void) {
    return i2s_sr.current_state;
}

/*
 * Push a 24-bit sample to the I2S peripheral
 * Data format: [0][Reg2][Reg1][Reg0] in a 32-bit word
 */
void i2s_shift_reg_push_sample(uint32_t state) {
    if (!i2s_sr.initialized) {
        return;
    }

    // Prepare 32-bit sample with 24-bit data
    // ESP32-S3 I2S expects 32-bit samples, we use lower 24 bits
    uint32_t sample = state & 0xFFFFFF;
    
    // Send sample via I2S DMA
    size_t bytes_written = 0;
    esp_err_t ret = i2s_write(I2S_NUM_0, &sample, sizeof(sample), &bytes_written, 0);
    
    if (ret != ESP_OK) {
        grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Error, "I2S write failed: %s", esp_err_to_name(ret));
    }
}

/*
 * Generate step pulse for a specific motor
 * motor_num: 0=X, 1=Y, 2=Z, 3=A
 * direction: true=forward, false=reverse
 */
void i2s_shift_reg_step_pulse(uint8_t motor_num, bool direction) {
    if (!i2s_sr.initialized || motor_num > 3) {
        return;
    }

    // Calculate bit positions for this motor
    uint8_t step_bit = motor_num * 2;     // 0,2,4,6 for X,Y,Z,A step
    uint8_t dir_bit = step_bit + 1;       // 1,3,5,7 for X,Y,Z,A direction

    // Set direction first
    i2s_shift_reg_write(dir_bit, direction);
    
    // Step pulse: high then low
    i2s_shift_reg_write(step_bit, true);   // Step high
    
    // Small delay for step pulse width (could be hardware timer based)
    delayMicroseconds(2);
    
    i2s_shift_reg_write(step_bit, false);  // Step low
    
    i2s_sr.step_count++;
}

#endif // CONFIG_IDF_TARGET_ESP32S3
