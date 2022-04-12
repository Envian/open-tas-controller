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

#include "n64.h"
#include "n64.pio.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/irq.h>
#include <hardware/pio.h>
#include <hardware/timer.h>

#include "global.h"
#include "helpers.h"


// REFERENCE: https://kthompson.gitlab.io/2016/07/26/n64-controller-protocol.html
#define getBit(reading) (reading == 0b11)

n64::DataPacket last_read;
volatile bool is_updated;

void handler_n64_debug() {
    uint skip_bit = 0;
    uint bytes = 0;
    uint data = 0;
    
    for (uint x = 0; x < 8; x++) {
        last_read.command <<= 1;
        last_read.command |= getBit(pio_sm_get_blocking(pio0, 0));
    }
    
    switch (last_read.command) {
        case 0x02: // Command includes 2 extra bytes denoting the memory address.
            skip_bit = 2 * 8;
            break;
        case 0x03:
            skip_bit = (2 + 32) * 8;
            break;
    }
    
    uint last_bit = time_us_32();
    last_read.timestamp = last_bit;
    last_read.bits = 0;
    
    // Stop reading after 32ms of no response. (4ms per bit, with a delay for reading from pack)
    do {
        if (!pio_sm_is_rx_fifo_empty(pio0, 0)) {
            uint reading = pio_sm_get(pio0, 0);
            if (reading == 1) {
                break;
            }
            last_bit = time_us_32();
            
            // Ignore the handover bit.
            if (last_read.bits == skip_bit) {
                skip_bit = -1;
            } else {
                data <<= 1;
                data |= getBit(reading);
                last_read.bits++;
                
                if (!(last_read.bits % 8)) {
                    last_read.data[bytes] = data;
                    bytes++;
                }
            }
        }
    } while (time_us_32() - last_bit < 32);
    
    is_updated = true;
    irq_clear(0);
}

namespace n64 {
    void read() {
        irq_set_exclusive_handler(PIO0_IRQ_0, &handler_n64_debug);
        irq_set_enabled(PIO0_IRQ_0, true);
        pio_set_irq0_source_enabled(pio0, pis_sm0_rx_fifo_not_empty, true);
        
        uint offset = pio_add_program(pio0, &n64_read_program);
        n64_read_program_install(pio0, 0, offset, 0);
        pio_sm_set_enabled(pio0, 0, true);
    }
    
    void debug_print() {
        while (true) {
            while (!is_updated) { }
            is_updated = false;
            
            print_int_hex(last_read.timestamp);
            putchar(',');
            putchar('0' + last_read.source);
            putchar(',');
            print_byte_hex(last_read.command);
            putchar(',');
            print_byte_hex(last_read.bits / 4);
            putchar(',');
            print_bytes_hex((uint8_t*)last_read.data, last_read.bits / 8);
            putchar('\n');
        }
    }
}
