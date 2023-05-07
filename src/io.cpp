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

#include <stdio.h>
#include "io.h"

#include "strings.h"
#include "loop_queue.h"

#define MAINCORE (get_core_num() == 0)

namespace io {
    LoopQueue<uint8_t, 256, 0> message_buffer;

    void write(uint8_t data) {
        if (MAINCORE) {
            putchar_raw(data);
        } else {
            message_buffer.add(data);
        }
    }

    void write(const uint8_t* data, uint count) {
        if (MAINCORE) {
            for (uint x = 0; x < count; x++) {
                putchar_raw(*(data+x));
            }
        } else {
            message_buffer.add(data, count);
        }
    }
    
    void writestr(const char* message) {
        uint8_t value = *message;
        while (value) {
            write(value);
            message++;
            value = *message;
        }
    }

    void endstr() {
        write('\n');
    }

    uint8_t read_blocking() {
        if (MAINCORE) {
            int data;
            do {
                // TODO: Maybe its not appropriate to send data from core1 in this method.
                while (message_buffer.gets_avaiable()) {
                    putchar_raw(message_buffer.get());
                }

                data = getchar_timeout_us(0);
            } while (data == PICO_ERROR_TIMEOUT);
            
            return data;
        } else {
            fail(strings::ERROR_SERIAL_READ_CORE1);
            return 0;
        }
    }
    
    void flush() {
        stdio_flush();
    }
}
