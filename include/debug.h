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

#include "helpers.h"
#include "hardware/gpio.h"

#define DEBUG_PIN 15
#define DEBUG_PUT(a) putchar(a); putchar('\n');
#define DEBUG_PRINT_VAR(val) print(#val ": "); print_int_hex(val); putchar('\n');
#define DEBUG_PIN_ON() gpio_put(DEBUG_PIN, 1);
#define DEBUG_PIN_OFF() gpio_put(DEBUG_PIN, 0);

void debug_init();
