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
#include <pico/stdlib.h>

namespace commands {
    namespace device {
        enum Device: uint8_t {
            NOP = 0x00,

            // Datastreams
            DATASTREAM_REQUEST = 0x80,
            DATASTREAM_STATUS = 0x81,

            // Messaging
            DEBUG = 0xFC,
            INFO = 0xFD,
            WARN = 0xFE,
            ERROR = 0xFF,
        };
    };
    namespace host {
        enum Host: uint8_t {
            NOP = 0x00,

            // Datastreams
            DATASTREAM_DATA = 0x80,
            CONTROLLER_CONFIG = 0x81
        };
    }
}