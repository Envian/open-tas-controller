// Open TAS Controller - Connects to game consoles via a Raspberry Pi Pico// Open TAS Controller - Connects to game consoles via a Raspberry Pi Pico
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

#define ERROR(message) static_assert(false, message);

#include <stdio.h>
#include "helpers.h"
#include "hardware/gpio.h"

#define TEMP_PUT(a) putchar(a); putchar('\n');
#define TEMP_PRINT_VAR(val) print(#val ": "); print_int_hex(val); putchar('\n');
#define TEMP_PIN_ON() gpio_put(15, 1);
#define TEMP_PIN_OFF() gpio_put(15, 0);

void debug_init();