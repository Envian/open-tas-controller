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

#include "devices.h"
#include "labels.h"

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
    CommandWriter& CommandWriter::write_bytes(CircularQueue<byte>* data, int count) {
        for (int x = 0; x < count; x++) {
            putchar_raw(data->get());
        }
        return *this;
    }
    CommandWriter& CommandWriter::write_str(const char* message) { 
        while (*message) {
            CommandWriter::write_byte(*message);
            message++;
        }
        return *this;
    }


    LogWriter::LogWriter(commands::device::Command command, const char* msg) 
    : CommandWriter(command) {
        write(msg);
    }
    LogWriter::~LogWriter() {
        CommandWriter::write_byte('\n');
    }

    LogWriter& LogWriter::write(const char* message) { 
        CommandWriter::write_byte(' ');
        while (*message) {
            CommandWriter::write_byte(*message);
            message++;
        }
        return *this;
    }
    LogWriter& LogWriter::write_byte(byte data) {
        CommandWriter::write_byte(' ');
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >> 4) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >> 0) & 0xF]);
        return *this;
    }
    LogWriter& LogWriter::write_short(uint16_t data) {
        CommandWriter::write_byte(' ');
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >> 12) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >>  8) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >>  4) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >>  0) & 0xF]);
        return *this;
    }
    LogWriter& LogWriter::write_int(uint32_t data) {
        CommandWriter::write_byte(' ');
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >> 28) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >> 24) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >> 20) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >> 16) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >> 12) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >>  8) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >>  4) & 0xF]);
        CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(data >>  0) & 0xF]);
        return *this;
    }
    LogWriter& LogWriter::write_bytes(const byte* data, int count) {
        CommandWriter::write_byte(' ');
        for (int x = 0; x < count; x++) {
            CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(*(data + x) >> 4) & 0xF]);
            CommandWriter::write_byte(NIBLE_CHARACTER_MAPPING[(*(data + x) >> 0) & 0xF]);
        }
        return *this;
    }
    void LogWriter::send() {}

    Debug::Debug(const char* message) : LogWriter(commands::device::DEBUG, message) {};
    Info::Info(const char* message) : LogWriter(commands::device::INFO, message) {};
    Warn::Warn(const char* message) : LogWriter(commands::device::WARN, message) {};
    Error::Error(const char* message) : LogWriter(commands::device::ERROR, message) {};
}
