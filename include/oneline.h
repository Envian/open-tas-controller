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

#pragma once
#include "global.h"

#include <hardware/pio.h>
#include <hardware/irq.h>

#define ONELINE_PIO pio0
#define ONELINE_IRQ PIO0_IRQ_0

namespace oneline {
    enum Port {
        port_1 = 0,
        port_2 = 1,
        port_3 = 2,
        port_4 = 3,
        port_invalid = -1
    };
    
    void set_handler(irq_handler_t handler);
    void init();
    
    Port get_port();
    int read_byte_blocking(Port port);
    int read_bytes_blocking(uint8_t buffer[], Port port, int count, int console_bytes);
    void read_discard(Port port);
    
    void write_bytes(Port port, uint8_t buffer[], int bytes);
}
