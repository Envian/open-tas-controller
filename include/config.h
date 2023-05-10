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

#define ONELINE_PIN_PORT_1 6
#define ONELINE_PIN_PORT_2 7
#define ONELINE_PIN_PORT_3 26
#define ONELINE_PIN_PORT_4 27

// Each byte of data takes 32us to transmit.
#define ONELINE_READ_TIMEOUT_US 48

// Activity LED:
// #define LED_SHOWS_ONELINE_ACTIVITY
#define LED_SHOWS_DATASTREAM_STATUS