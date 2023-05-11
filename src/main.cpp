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

#include "base_device.h"
#include "nintendo/n64_datastream.h"

#include "io.h"

#include <hardware/gpio.h>

int main() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    stdio_init_all();
    
    // Wait for a single byte before starting.
    io::read_blocking();

    current_device = new n64::Datastream();

    while(true) {
        // The blocking loop for reading will update the device.
        switch (io::read_blocking()) {
        case commands::host::NOP: break;
        case commands::host::DATASTREAM_DATA:
            current_device->handle_datastream();
            break;
        case commands::host::CONTROLLER_CONFIG:
            current_device->handle_controller_config();
            break;
        }
    }
}
