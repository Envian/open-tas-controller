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

#define __LOGGER(__name, __cmd, __prefix)\
class __name : public io::LogWriter {\
public:\
    __name(const char* msg) : LogWriter(__cmd, __prefix, msg) {}\
};

#define LOGGERS(prefix) \
__LOGGER(Debug, commands::device::DEBUG, prefix)\
__LOGGER(Info, commands::device::INFO, prefix)\
__LOGGER(Warn, commands::device::WARN, prefix)\
__LOGGER(Error, commands::device::ERROR, prefix)

namespace io {
    uint8_t read_blocking();

    // Message Writers
    class CommandWriter {
    public:
        CommandWriter(commands::device::Command command);
        
        CommandWriter& write_byte(uint8_t data);
        CommandWriter& write_short(uint16_t data);
        CommandWriter& write_int(uint32_t data);
        CommandWriter& write_bytes(const uint8_t *data, uint count);

        CommandWriter& write_str(const char* message);
        CommandWriter& write_str_byte(uint8_t data);
        CommandWriter& write_str_short(uint16_t data);
        CommandWriter& write_str_int(uint32_t data);

        virtual void send();
    };

    class LogWriter : public CommandWriter {
    public:
        LogWriter(commands::device::Command command, const char* prefix, const char* msg);
        void send() override;
    };

    LOGGERS("[Core]");
}
