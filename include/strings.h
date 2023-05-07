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

namespace strings {

    // Errors
    static constexpr char ERROR_UNKNOWN[] = "UNKNOWN";
    static constexpr char ERROR_SERIAL_READ_CORE1[] = "CORE1_SERIAL_READ";
    static constexpr char ERROR_DATASTREAM_UNDERFLOW[] = "DATASTREAM_UNDERFLOW";
    static constexpr char ERROR_DATASTREAM_OVERFLOW[] = "DATASTREAM_OVERFLOW";
}