// Open TAS Controller - Connects to game consoles via a Raspberry Pi Pico
// Copyright (C) 2022  Russell Small
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "oneline.h"
#include "oneline.pio.h"

#include <hardware/pio.h>
#include <hardware/clocks.h>

// PIO data can only be 8 bits due to pushing 1 bit at a time and auto push.
// To assist with reading all data from the N64, after all data is received,
// the PIO program sends the number of bits read + BIT_COUNT_FLAG.
#define BIT_COUNT_FLAG (1 << 31)

namespace oneline {
    uint oneline_offset = 0;
    
    inline void abort_read(uint controller) {
        pio_sm_exec(ONELINE_PIO, controller, pio_encode_jmp(oneline_offset + oneline_offset_reset_bit));
    }
    
    void setup_controller(uint controller, uint pin) {
        pio_gpio_init(ONELINE_PIO, pin);
        pio_sm_set_consecutive_pindirs(ONELINE_PIO, controller, pin, 1, false);
        pio_set_irq0_source_enabled(ONELINE_PIO, (pio_interrupt_source)(pis_interrupt0 + controller), true);
        
        pio_sm_config config = oneline_program_get_default_config(oneline_offset);
        sm_config_set_clkdiv(&config, (float)clock_get_hz(clk_sys) / (float)oneline_F_PIO);
        
        sm_config_set_in_pins(&config, pin);
        sm_config_set_out_pins(&config, pin, 1);
        sm_config_set_jmp_pin(&config, pin);
        
        sm_config_set_in_shift(&config, false /*shift right*/, true /*auto push*/, 8 /*push size*/);
        sm_config_set_out_shift(&config, true /*shift right*/, true /*auto pull*/, 8 /*pull size*/);
        
        pio_sm_init(ONELINE_PIO, controller, oneline_offset, &config);
        
        // Program depends on X being defaulted to 0x8FFFFFFF
        pio_sm_put(ONELINE_PIO, controller, ~BIT_COUNT_FLAG);
        pio_sm_exec(ONELINE_PIO, controller, pio_encode_out(pio_x, 32));
        
        pio_sm_set_enabled(ONELINE_PIO, controller, true);
    }
    
    void init() {
        oneline_offset = pio_add_program(ONELINE_PIO, &oneline_program);
        irq_set_enabled(ONELINE_IRQ, true);
        
        setup_controller(0, ONELINE_PIN_CONTROLLER_0);
        setup_controller(1, ONELINE_PIN_CONTROLLER_1);
        setup_controller(2, ONELINE_PIN_CONTROLLER_2);
        setup_controller(3, ONELINE_PIN_CONTROLLER_3);
    }
    
    uint get_controller() {
        for (uint n = 0; n < 4; n++) {
            if (pio_interrupt_get(ONELINE_PIO, n)) {
                return n;
            }
        }
        return -1;
    }
    
    uint read_byte_blocking(uint controller) {
        uint last_activity = time_us_32();
        do {
            if (!pio_sm_is_rx_fifo_empty(ONELINE_PIO, controller)) {
                uint data = pio_sm_get(ONELINE_PIO, controller);
                
                if (data < BIT_COUNT_FLAG) {
                    return data;
                } else {
                    return -1;
                }
            }
        }
        while (time_us_32() - last_activity < ONELINE_READ_TIMEOUT_US);
        
        return -1;
    }
    
    uint read_bytes_blocking(uint8_t buffer[], uint controller, uint count, uint console_bytes) {
        uint bytes = 0;
        uint last_activity = time_us_32();
        uint data;
        uint reported_bits = 0;
        
        // Step 1: Read all data from pio.
        while(true) {
            if (!pio_sm_is_rx_fifo_empty(ONELINE_PIO, controller)) {
                uint data = pio_sm_get(ONELINE_PIO, controller);
                if (data >= BIT_COUNT_FLAG) {
                    // Safety so we don't rotate out of our memory space.
                    // Left align the last few bits received.
                    if (bytes > 0) {
                        buffer[bytes - 1] <<= 8 - ((data & ~BIT_COUNT_FLAG) % 8);
                    }
                    reported_bits = data & ~BIT_COUNT_FLAG;
                    break;
                }
                
                // discard data that would otherwise overflow.
                if (bytes < count) {
                    buffer[bytes] = (uint8_t)(data & 0xFF);
                }
                bytes++;
                last_activity = time_us_32();
            } else if (time_us_32() - last_activity >= ONELINE_READ_TIMEOUT_US) {
                // HACK: This does not guarantee the loop will break.
                // but, it should force the pio program to dump its current data.
                abort_read(controller);
            }
        }

        
        // Step 2: Rotate
        data = 0;
        for (uint n = min(bytes, count); n > console_bytes; n--) {
            data = (buffer[n-1] << 1) | (data >> 8);
            buffer[n-1] = (uint8_t)(data & 0xFF);
        }
        
        return reported_bits;
    }
}