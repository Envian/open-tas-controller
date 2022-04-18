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

#include "helpers.h"

#include <pico/stdlib.h>
#include <stdio.h>

char NIBLE_CHARACTER_MAPPING[] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};

void print_byte_hex(uint data) {
    putchar(NIBLE_CHARACTER_MAPPING[(data >> 4) & 0xF]);
    putchar(NIBLE_CHARACTER_MAPPING[data & 0xF]);
}

void print_short_hex(uint data) {
    print_byte_hex(data >> 8);
    print_byte_hex(data);
}

void print_int_hex(uint data) {
    print_short_hex(data >> 16);
    print_short_hex(data);
}

void print_bytes_hex(uint8_t data[], uint count) {
    for (uint n = 0; n < count; n++) {
        print_byte_hex(data[n]);
    }
}

void print(char *string) {
    while (*string) {
        putchar(*string);
        string++;
    }
}
