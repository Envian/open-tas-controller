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
#include "commands.h"

namespace io {
    byte read_blocking();

    // Message Writers
    class CommandWriter {
    public:
        CommandWriter(commands::device::Command command);
        
        CommandWriter& write_byte(byte data);
        CommandWriter& write_short(uint16_t data);
        CommandWriter& write_int(uint32_t data);
        CommandWriter& write_bytes(const byte *data, int count);
        CommandWriter& write_str(const char *data);
    };

    class LogWriter : private CommandWriter {
    public:
        LogWriter(commands::device::Command command, const char* msg);
        ~LogWriter();

        LogWriter& write(const char* message);
        LogWriter& write_byte(byte data);
        LogWriter& write_short(uint16_t data);
        LogWriter& write_int(uint32_t data);
        LogWriter& write_bytes(const byte* data, int count);
        void send(); // Dummy method, for semantics

        // Prevent log writers from being copied, calling destructor early
        LogWriter(const LogWriter&) = delete;
        LogWriter& operator=(const LogWriter&) = delete;
    };

    class Debug : public LogWriter {
    public:
        Debug(const char* message);

        Debug(const Debug&) = delete;
        Debug& operator=(const Debug&) = delete;
    };

    class Info : public LogWriter {
    public:
        Info(const char* message);

        Info(const Info&) = delete;
        Info& operator=(const Info&) = delete;
    };

    class Warn : public LogWriter {
    public:
        Warn(const char* message);

        Warn(const Warn&) = delete;
        Warn& operator=(const Warn&) = delete;
    };

    class Error : public LogWriter {
    public:
        Error(const char* message);

        Error(const Error&) = delete;
        Error& operator=(const Error&) = delete;
    };
}
