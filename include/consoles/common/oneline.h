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

#include "base_device.h"

#include <hardware/pio.h>
#include <hardware/irq.h>

namespace oneline {
    // Note: Many features depend on port 1 being 0 for array indexing & pio.
    enum Port {
        port_1 = 0,
        port_2 = 1,
        port_3 = 2,
        port_4 = 3,
        port_invalid = -1
    };

    class OnelineHandler {
    public:
        virtual void handle_oneline(Port port) = 0;
    };

    void init(OnelineHandler* handler);
    void uninit();

    int read_byte_blocking(Port port);
    int read_bytes_blocking(byte buffer[], Port port, int count, int request_bytes);
    void read_discard(Port port);

    // void write_request(Port port, const byte buffer[], int bytes);
    // void write_reply(Port port, const byte buffer[], int bytes);

    class Writer {
    public:
        Writer(Port port, int count);
        // void begin_response(int bytes);
        // void begin_request(int bytes);
        Writer& write(byte data);
        Writer& write(const byte* buffer);
        Writer& write(const byte* buffer, int count);
        Writer& write_zeros();
    private:
        const Port port;
        const int bytes;
        int written;
        uint32_t data;
    };
}
