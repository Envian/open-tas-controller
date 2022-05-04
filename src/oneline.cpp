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
#define BIT_COUNT_FLAG (uint)(1 << 31)

namespace oneline {
    uint pio_offset = 0;
    
    void setup_controller(Controller controller, uint pin) {
        pio_gpio_init(ONELINE_PIO, pin);
        pio_sm_set_consecutive_pindirs(ONELINE_PIO, (uint)controller, pin, 1, false);
        pio_set_irq0_source_enabled(ONELINE_PIO, (pio_interrupt_source)(pis_interrupt0 + (uint)controller), true);
        
        pio_sm_config reader_config = oneline_program_get_default_config(pio_offset);
        sm_config_set_clkdiv(&reader_config, (float)clock_get_hz(clk_sys) / (float)oneline_F_PIO);
        
        sm_config_set_in_pins(&reader_config, pin);
        sm_config_set_out_pins(&reader_config, pin, 1);
        sm_config_set_set_pins(&reader_config, pin, 1);
        sm_config_set_jmp_pin(&reader_config, pin);
        
        sm_config_set_in_shift(&reader_config, false /*shift right*/, false /*auto push*/, 8 /*push size*/);
        sm_config_set_out_shift(&reader_config, false /*shift left*/, false /*auto pull*/, 32 /*pull size*/);
        
        pio_sm_init(ONELINE_PIO, (uint)controller, pio_offset, &reader_config);
        // Reader depends on X being defaulted to 0x8FFFFFFF
        pio_sm_put(ONELINE_PIO, (uint)controller, ~BIT_COUNT_FLAG);
        pio_sm_exec_wait_blocking(ONELINE_PIO, (uint)controller, pio_encode_pull(false, true));
        pio_sm_exec_wait_blocking(ONELINE_PIO, (uint)controller, pio_encode_out(pio_x, 32));
        
        pio_sm_set_enabled(ONELINE_PIO, (uint)controller, true);
    }
    
    void init() {
        pio_offset = pio_add_program(ONELINE_PIO, &oneline_program);
        irq_set_enabled(ONELINE_IRQ, true);
        
        setup_controller(controller_1, ONELINE_PIN_PORT_0);
        setup_controller(controller_2, ONELINE_PIN_PORT_1);
        setup_controller(controller_3, ONELINE_PIN_PORT_2);
        setup_controller(controller_4, ONELINE_PIN_PORT_3);
    }
    
    Controller get_controller() {
        for (int n = 0; n < 4; n++) {
            if (pio_interrupt_get(ONELINE_PIO, n)) {
                return (Controller)n;
            }
        }
        return controller_invalid;
    }
    
    // --------------------
    // |     READING      |
    // --------------------
    
    inline void abort_read(Controller controller) {
        pio_sm_exec(ONELINE_PIO, controller, pio_encode_jmp(pio_offset + oneline_offset_reset_bit));
    }
    
    int __time_critical_func(read_byte_blocking)(Controller controller) {
        uint last_activity = time_us_32();
        do {
            if (!pio_sm_is_rx_fifo_empty(ONELINE_PIO, (uint)controller)) {
                uint data = pio_sm_get(ONELINE_PIO, (uint)controller);
                return (data < BIT_COUNT_FLAG) ? data : -1;
            }
        }
        while (time_us_32() - last_activity < ONELINE_READ_TIMEOUT_US);
        
        return -1;
    }
    
    uint __time_critical_func(read_bytes_blocking)(uint8_t buffer[], Controller controller, uint count, uint console_bytes) {
        uint bytes = 0;
        uint last_activity = time_us_32();
        uint data;
        uint reported_bits = 0;
        
        // Step 1: Read all data from pio.
        while(true) {
            if (!pio_sm_is_rx_fifo_empty(ONELINE_PIO, (uint)controller)) {
                uint data = pio_sm_get(ONELINE_PIO, (uint)controller);
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
            } 
            else if (time_us_32() - last_activity >= ONELINE_READ_TIMEOUT_US) {
                abort_read(controller);
                last_activity = time_us_32();
            }
        }

        
        // Step 2: Rotate bits to their correct position.
        data = 0;
        for (uint n = min(bytes, count); n > console_bytes; n--) {
            data = (buffer[n-1] << 1) | (data >> 8);
            buffer[n-1] = (uint8_t)(data & 0xFF);
        }
        
        return reported_bits;
    }
    
    void __time_critical_func(read_discard)(Controller controller) {
        uint last_activity = time_us_32();
        uint data = 0;

        while(data < BIT_COUNT_FLAG) {
            if (!pio_sm_is_rx_fifo_empty(ONELINE_PIO, (uint)controller)) {
                data = pio_sm_get(ONELINE_PIO, (uint)controller);
                last_activity = time_us_32();
            } 
            else if (time_us_32() - last_activity >= ONELINE_READ_TIMEOUT_US) {
                abort_read(controller);
                last_activity = time_us_32();
            }
        }
    }
    
    // --------------------
    // |     WRITING      |
    // --------------------
    
    void __time_critical_func(write_bytes)(Controller controller, uint buffer[], uint bytes) {
        pio_sm_put(ONELINE_PIO, (uint)controller, bytes * 8);
        pio_sm_exec(ONELINE_PIO, (uint)controller, pio_encode_jmp(pio_offset + oneline_offset_write));
        
        for (uint words_written = 0; words_written * 4 < bytes; words_written++) {
            pio_sm_put_blocking(ONELINE_PIO, (uint)controller, ~buffer[words_written]);
        }
    }
}
