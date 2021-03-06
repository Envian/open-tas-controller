// Open TAS Controller - A controller emulator for running TAS on hardware.
// Copyright (C) 2019  Russell Small
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

#include <Arduino.h>

#include "config.h"
#include "consoles.h"
#include "helpers.h"

void setup() {
	Serial.begin(SERIAL_BAUD);
	Serial.write("Ready");
}

void loop() {
	switch (Serial.read()) {
	case 0xA: // Begin Playback
		runConsole((PlaybackMode)Helpers::readBlocking());
		break;
	case 'd': // Describe
		Serial.write("Open TAS Controller\n");
		break;
	case 'v': // Version
		Serial.write(STR(VERSION) "\n");
		break;
	}
}
