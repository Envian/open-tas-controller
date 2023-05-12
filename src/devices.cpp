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

#include "devices.h"
#include "io.h"
#include "labels.h"

#define UNSUPPORTED_DEVICE(DEVICE) current_device = new DummyDevice();\
io::Error(labels::ERROR_UNSUPPORTED_DEVICE).write(DEVICE);

#define UNKNOWN_MODE(DEVICE, MODE) current_device = new DummyDevice();\
io::Error(labels::ERROR_UNKNOWN_MODE).write(DEVICE).write_byte(MODE);

BaseDevice *current_device = new DummyDevice();

enum DeviceType {
    PLAYBACK = 0,
    RECORD = 1,
    REALTIME = 2,
    DEVICE_SPECIFIC_1 = 3,
};

#define MAKE_ID(VALUE) (((uint32_t)VALUE[0] << 16) | ((uint32_t)VALUE[1] << 8) | ((uint32_t)VALUE[2]))

#ifdef N64_SUPPORT
#include "nintendo/n64_datastream.h"
#endif

void load_new_device() {
    delete current_device;
    current_device = nullptr;

    // Need to compare 3 bytes. Load them into device_identifier and cast that to an int.
    byte device_identifier[4];
    device_identifier[0] = io::read_blocking();
    device_identifier[1] = io::read_blocking();
    device_identifier[2] = io::read_blocking();
    device_identifier[3] = 0; // this is so we can use this as a string.

    byte device_type = io::read_blocking();

    switch (MAKE_ID(device_identifier)) {
    case MAKE_ID(labels::CONSOLE_N64):
#ifdef N64_SUPPORT
        switch (device_type) {
            case DEVICE_SPECIFIC_1: 
            current_device = new n64::Datastream();
            break;
        default:
            UNKNOWN_MODE(labels::CONSOLE_N64, device_type);
            break;
        }
#else
        UNSUPPORTED_DEVICE(labels::CONSOLE_N64);
#endif
        break;
    default:
        io::Error(labels::ERROR_UNKNOWN_DEVICE).write((const char*)device_identifier);
        break;
    }

}