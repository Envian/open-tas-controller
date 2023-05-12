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
#include "global.h"

namespace commands {
    namespace device {
        enum Command: byte {
            NOP = 0x00,
            // All replies from 0x01 to 0x79 should be treated as text
            REPLY = 0x02,

            // 0xD0-0xDF - Datastream Commands
            DATASTREAM_REQUEST = 0xD0,
            DATASTREAM_STATUS = 0xD1,

            // 0xF0-0xFF - Text/Info Commands
            ACKNOWLEDGE = 0xF0,
            DEBUG = 0xFC,
            INFO = 0xFD,
            WARN = 0xFE,
            ERROR = 0xFF,
        };
    };
    namespace host {
        enum Command: byte {
            // 0x00 - 0x7F General Inqueries
            // For these, use ASCII when available
            NOP = 0x00,
            NOP_CR = '\r', 
            NOP_LF = '\n', 

            // Basic Inqueries
            INFO = '?',
            INFO_ALT = 'h',
            INFO_ALT2 = 'H',
            VERSION = 'V',

            // 0x80-0x8F - Top Level Configuration
            SET_DEVICE = 0x80,
            STOP_DEVICE = 0x81,

            // 0x90-0xAF - Device Configuration

            // 0xB0-0xBF - Reader Commands

            // 0xC0-0xCF - Writer Commands
            
            // 0xD0-0xDF - Datastream Commands
            DATASTREAM_DATA = 0xD0,
            CONTROLLER_CONFIG = 0xD1,
        };
    };
}