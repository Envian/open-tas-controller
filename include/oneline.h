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
    enum Controller {
        controller_1 = 0,
        controller_2 = 1,
        controller_3 = 2,
        controller_4 = 3,
        controller_invalid = -1
    };
    
    inline void set_handler(irq_handler_t handler) { 
        irq_set_exclusive_handler(ONELINE_IRQ, handler); 
    };
    void init();
    
    Controller get_controller();
    int read_byte_blocking(Controller controller);
    uint read_bytes_blocking(uint8_t *buffer, Controller controller, uint max, uint console_bytes);
    void read_discard(Controller controller);
    
    void write_bytes(Controller controller, uint buffer[], uint bytes);
}
