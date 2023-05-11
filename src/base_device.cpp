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

#include <stdio.h>
#include "base_device.h"

BaseDevice *current_device;

BaseDevice::BaseDevice() {}

BaseDevice::~BaseDevice() {}

void BaseDevice::update() {}

bool BaseDevice::is_oneline() const {
    return false;
}

void BaseDevice::handle_datastream() {
    // TODO: Report Error
}

void BaseDevice::handle_controller_config() {
    // TODO: Report Error

}
