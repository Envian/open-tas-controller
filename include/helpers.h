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

#define LED_ON() gpio_put(PICO_DEFAULT_LED_PIN, 1);
#define LED_OFF() gpio_put(PICO_DEFAULT_LED_PIN, 0);

void print_byte_hex(uint data);
void print_short_hex(uint data);
void print_int_hex(uint data);
void print_bytes_hex(uint8_t data[], int count);

void print(char *string);

void fast_wait_us(uint duration);
