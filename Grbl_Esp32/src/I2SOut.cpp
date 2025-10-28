/*
    I2SOut.cpp
    Part of Grbl_ESP32

    Basic GPIO expander using the ESP32 I2S peripheral (output)

    2020    - Michiyasu Odaki

    Grbl_ESP32 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Grbl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grbl_ESP32.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Config.h"
#include "Pins.h"
#include "I2SOut.h"

// Include ESP32-S3 specific I2S implementation
#ifdef CONFIG_IDF_TARGET_ESP32S3
#include "I2SOut_ESP32S3.h"

// Forward ESP32-S3 shift register functions to standard I2S interface
int i2s_out_init(i2s_out_init_t& init_param) { 
    return i2s_shift_reg_init(); 
}

int i2s_out_init() { 
    return i2s_shift_reg_init(); 
}

uint8_t i2s_out_read(uint8_t pin) { 
    if (pin >= I2S_OUT_PIN_BASE && pin < I2S_OUT_PIN_BASE + I2S_SHIFT_REG_TOTAL_BITS) {
        return i2s_shift_reg_read(pin - I2S_OUT_PIN_BASE) ? 1 : 0;
    }
    return 0; 
}

void i2s_out_write(uint8_t pin, uint8_t val) {
    if (pin >= I2S_OUT_PIN_BASE && pin < I2S_OUT_PIN_BASE + I2S_SHIFT_REG_TOTAL_BITS) {
        i2s_shift_reg_write(pin - I2S_OUT_PIN_BASE, val != 0);
    }
}

uint32_t i2s_out_push_sample(uint32_t usec) { 
    // For ESP32-S3, this is handled internally by the I2S driver
    return 0; 
}

int i2s_out_set_passthrough() { return 0; }
int i2s_out_set_stepping() { return 0; }
void i2s_out_delay() {}
int i2s_out_set_pulse_period(uint32_t usec) { return 0; }
int i2s_out_set_pulse_callback(i2s_out_pulse_func_t func) { return 0; }

int i2s_out_reset() { 
    return i2s_shift_reg_deinit(); 
}

i2s_out_pulser_status_t i2s_out_get_pulser_status() {
    // Return PASSTHROUGH for ESP32-S3 shift register mode
    return PASSTHROUGH;
}

#else // ESP32 classic - provide stubs for now

// For ESP32 classic, we would include the full original implementation
// But for now, let's just provide the same stubs to ensure compilation works
#warning "TODO: Add full ESP32 classic I2S implementation"

int i2s_out_init(i2s_out_init_t& init_param) { return 0; }
int i2s_out_init() { return 0; }
uint8_t i2s_out_read(uint8_t pin) { return 0; }
void i2s_out_write(uint8_t pin, uint8_t val) {}
uint32_t i2s_out_push_sample(uint32_t usec) { return 0; }
int i2s_out_set_passthrough() { return 0; }
int i2s_out_set_stepping() { return 0; }
void i2s_out_delay() {}
int i2s_out_set_pulse_period(uint32_t usec) { return 0; }
int i2s_out_set_pulse_callback(i2s_out_pulse_func_t func) { return 0; }
int i2s_out_reset() { return 0; }

#endif
