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

#include "io.h"

#include <stdio.h>

#include "base_device.h"
#include "labels.h"

#define MAINCORE (get_core_num() == 0)

static constexpr char NIBLE_CHARACTER_MAPPING[] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};

namespace io {
    byte read_blocking() {
        int data;
        do {
            if (current_device) { current_device->update(); }
            data = getchar_timeout_us(0);
        } while (data == PICO_ERROR_TIMEOUT);
        
        return data;
    }
    
    CommandWriter::CommandWriter(commands::device::Command command) {
        putchar_raw(command);
    }

    CommandWriter& CommandWriter::write_byte(byte data) {
        putchar_raw(data);
        return *this;
    }
    CommandWriter& CommandWriter::write_short(uint16_t data) {
        putchar_raw(data & 0xFF);
        putchar_raw((data >> 8) & 0xFF);
        return *this;
    }
    CommandWriter& CommandWriter::write_int(uint32_t data) {
        putchar_raw(data & 0xFF);
        putchar_raw((data >> 8) & 0xFF);
        putchar_raw((data >> 16) & 0xFF);
        putchar_raw((data >> 24) & 0xFF);
        return *this;
    }
    CommandWriter& CommandWriter::write_bytes(const byte* data, int count) {
        for (int x = 0; x < count; x++) {
            putchar_raw(*(data + x));
        }
        return *this;
    } 

    CommandWriter& CommandWriter::write_str(const char* message) { 
        while (*message) {
            putchar_raw(*message);
            message++;
        }
        return *this;
    }
    CommandWriter& CommandWriter::write_str_byte(byte data) {
        putchar_raw(NIBLE_CHARACTER_MAPPING[(data >> 4)]);
        putchar_raw(NIBLE_CHARACTER_MAPPING[data & 0xF]);
        return *this;
    }
    CommandWriter& CommandWriter::write_str_short(uint16_t data) {
        write_str_byte(data >> 8);
        write_str_byte(data);
        return *this;
    }
    CommandWriter& CommandWriter::write_str_int(uint32_t data) {
        write_str_short(data >> 16);
        write_str_short(data);
        return *this;
    }

    void CommandWriter::send() { }
    
    CommandWriter debug(const char* message) {
        return CommandWriter(commands::device::DEBUG).write_str(message);
    }
    CommandWriter info(const char* message) {
        return CommandWriter(commands::device::INFO).write_str(message);
    }
    CommandWriter warn(const char* message) {
        return CommandWriter(commands::device::WARN).write_str(message);
    }
    CommandWriter error(const char* message) {
        return CommandWriter(commands::device::ERROR).write_str(message);
    }


    LogWriter::LogWriter(commands::device::Command command, const char* prefix, const char* msg) 
    : CommandWriter(command) {
        write_str(prefix);
        write_str(msg);
    }

    void LogWriter::send() {
        write_byte('\n');
    }
}
