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

#include "global.h"

#include <hardware/gpio.h>

#include "io.h"
#include "labels.h"
#include "devices.h"


int main() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    stdio_init_all();
    
    while(true) {
        // The blocking loop for reading will update the device.
        byte cmd = io::read_blocking();
        switch (cmd) {
        case commands::host::NOP: 
        case commands::host::NOP_CR:
        case commands::host::NOP_LF:
            // Hides errors when interacting via serial.
            break;

        case commands::host::INFO:
        case commands::host::INFO_ALT:
        case commands::host::INFO_ALT2:
            io::CommandWriter(commands::device::REPLY)
                .write_str(labels::DEVICE_INFO).write_byte('\n');
            break;

        case commands::host::SET_DEVICE:
            load_new_device();
            break;

        case commands::host::DATASTREAM_DATA:
            current_device->handle_datastream();
            break;

        case commands::host::CONTROLLER_CONFIG:
            current_device->handle_controller_config();
            break;

        default:
            // Anything typeable should be considered the user typing in a serial program.
            if (cmd > 0x79) {
                io::Error(labels::ERROR_UNKNOWN_COMMAND).write_byte(cmd);
            }
        }
    }
}
