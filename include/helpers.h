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

#include "io.h"
#include "labels.h"
#include "commands.h"


#define LED_ON() gpio_put(PICO_DEFAULT_LED_PIN, 1);
#define LED_OFF() gpio_put(PICO_DEFAULT_LED_PIN, 0);

#define TIMED_OUT(start, duration) (time_us_32() - start >= duration)

void fast_wait_us(uint duration);

// Debug Methods (This currently doesn't work)
#define UNIMPLEMENTED { io::Error(labels::NOT_IMPLEMENTED).write_byte(':').write_str(__PRETTY_FUNCTION__).send(); }