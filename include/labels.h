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

namespace labels {
    static constexpr char CONSOLE_N64[] = "N64";

    static constexpr char DEVICE_TYPE_PLAYBACK[] = "PLAY";
    static constexpr char DEVICE_TYPE_RECORD[] = "RECORD";
    // Datastream is a variation of playback which can replay mupen movies.
    static constexpr char DEVICE_TYPE_DATASTREAM[] = "DATASTREAM";

    // PORT_INFO - Varies based on system.
    static constexpr char DEBUG_PORT_INFO[] = "PORT_INFO";
    
    // Infos
    // DEVICE_INITIALIZED - Console(3char) - Type
    static constexpr char INFO_DEVICE_INIT[] = "DEVICE_INIT";

    // Errors
}