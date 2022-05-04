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

namespace oneline {
    uint pio_offset = 0;
    
    // Shortcut Methods
    inline bool can_read(Port port) { return !pio_sm_is_rx_fifo_empty(ONELINE_PIO, (uint)port); }
    inline uint32_t read(Port port) { return pio_sm_get(ONELINE_PIO, (uint)port); }
    inline bool can_write(Port port) { return !pio_sm_is_tx_fifo_full(ONELINE_PIO, (uint)port); }
    inline void write(Port port, uint32_t data) { pio_sm_put(ONELINE_PIO, (uint)port, data); }
    inline void write_blocking(Port port, uint32_t data) { pio_sm_put_blocking(ONELINE_PIO, (uint)port, data); }
    inline void jump(Port port, uint offset) { pio_sm_exec(ONELINE_PIO, port, pio_encode_jmp(pio_offset + offset)); }
    inline void abort_read(Port port) { jump(port, oneline_offset_reset_bit); }
    inline void begin_write(Port port) { jump(port, oneline_offset_reset_bit); }

    void set_handler(irq_handler_t handler) { 
        irq_set_exclusive_handler(ONELINE_IRQ, handler); 
    };
    
    void setup_port(Port port, uint pin) {
        pio_gpio_init(ONELINE_PIO, pin);
        pio_sm_set_consecutive_pindirs(ONELINE_PIO, (uint)port, pin, 1, false);
        pio_set_irq0_source_enabled(ONELINE_PIO, (pio_interrupt_source)(pis_interrupt0 + (uint)port), true);
        
        pio_sm_config reader_config = oneline_program_get_default_config(pio_offset);
        sm_config_set_clkdiv(&reader_config, (float)clock_get_hz(clk_sys) / (float)oneline_F_PIO);
        
        sm_config_set_in_pins(&reader_config, pin);
        sm_config_set_out_pins(&reader_config, pin, 1);
        sm_config_set_set_pins(&reader_config, pin, 1);
        sm_config_set_jmp_pin(&reader_config, pin);
        
        sm_config_set_in_shift(&reader_config, false /*shift right*/, false /*auto push*/, 8 /*push size*/);
        sm_config_set_out_shift(&reader_config, false /*shift left*/, false /*auto pull*/, 32 /*pull size*/);
        
        pio_sm_init(ONELINE_PIO, (uint)port, pio_offset, &reader_config);
        // Reader depends on X being defaulted to 0xFFFFFFFF
        pio_sm_exec(ONELINE_PIO, (uint)port, pio_encode_mov_not(pio_x, pio_null));
        pio_sm_set_enabled(ONELINE_PIO, (uint)port, true);
    }
    
    void init() {
        pio_offset = pio_add_program(ONELINE_PIO, &oneline_program);
        irq_set_enabled(ONELINE_IRQ, true);
        
        setup_port(port_1, ONELINE_PIN_PORT_1);
        setup_port(port_2, ONELINE_PIN_PORT_2);
        setup_port(port_3, ONELINE_PIN_PORT_3);
        setup_port(port_4, ONELINE_PIN_PORT_4);
    }
    
    Port get_port() {
        if (pio_interrupt_get(ONELINE_PIO, (uint)port_1)) { return port_1; }
        if (pio_interrupt_get(ONELINE_PIO, (uint)port_2)) { return port_2; }
        if (pio_interrupt_get(ONELINE_PIO, (uint)port_3)) { return port_3; }
        if (pio_interrupt_get(ONELINE_PIO, (uint)port_4)) { return port_4; }
        return port_invalid;
    }
    
    // --------------------
    // |     READING      |
    // --------------------
    
    int __time_critical_func(read_byte_blocking)(Port port) {
        uint start_time = time_us_32();
        while (!TIMED_OUT(start_time, ONELINE_READ_TIMEOUT_US)) {
            if (can_read(port)) {
                uint data = read(port);
                return (data <= 0xFF) ? (int)data : -1;
            }
        }
        return -1;
    }
    
    int __time_critical_func(read_bytes_blocking)(uint8_t buffer[], Port port, int count, int console_bytes) {
        int bytes = 0;
        uint last_activity = time_us_32();
        uint data;
        int reported_bits = 0;
        
        // Step 1: Read all data from pio.
        while(true) {
            if (can_read(port)) {
                uint data = read(port);
                
                // Values higher than 255 represent the end of a command.
                // This is a bit-inverted counter of how many bits were read.
                if (data > 0xFF) {
                    // Safety so we don't rotate out of our memory space.
                    // Left align the last few bits received.
                    reported_bits = ~data;
                    if (bytes > 0) {
                        buffer[bytes - 1] <<= 8 - (reported_bits % 8);
                    }
                    break;
                }
                
                // discard data that would otherwise overflow.
                if (bytes < count) {
                    buffer[bytes] = (uint8_t)(data & 0xFF);
                }
                bytes++;
                last_activity = time_us_32();
            } 
            else if (TIMED_OUT(last_activity, ONELINE_READ_TIMEOUT_US)) {
                abort_read(port);
                last_activity = time_us_32();
            }
        }

        
        // Step 2: Rotate bits to their correct position.
        data = 0;
        for (int n = MIN(bytes, count); n > console_bytes; n--) {
            data = (buffer[n-1] << 1) | (data >> 8);
            buffer[n-1] = (uint8_t)(data & 0xFF);
        }
        
        return reported_bits;
    }
    
    void __time_critical_func(read_discard)(Port port) {
        uint last_activity = time_us_32();
        uint data = 0;

        while(data <= 0xFF) {
            if (can_read(port)) {
                data = read(port);
                last_activity = time_us_32();
            } 
            else if (TIMED_OUT(last_activity, ONELINE_READ_TIMEOUT_US)) {
                abort_read(port);
                last_activity = time_us_32();
            }
        }
    }
    
    // --------------------
    // |     WRITING      |
    // --------------------
    
    void __time_critical_func(write_bytes)(Port port, uint buffer[], int bytes) {
        write(port, bytes * 8);
        begin_write(port);
        
        for (int words_written = 0; words_written * 4 < bytes; words_written++) {
            // Because we write pindirs with a pull up resistor, this is inverted.
            write_blocking(port, ~buffer[words_written]);
        }
    }
}
