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
#include "nintendo/models.h"

#include "nintendo/oneline.h"
#include "loop_queue.h"

#define DATASTREAM_BUFFER_SIZE 64
#define N64_CONTROLLER_COUNT 4

namespace n64 {
    class n64_Datastream : public oneline::OnelineDevice {
    public:
        n64_Datastream();
        ~n64_Datastream();
        bool handle_datastream();
        bool handle_controller_config();
        void handle_oneline(oneline::Port port);
        void core1_loop();
    private:
        ControllerConfig controllers[N64_CONTROLLER_COUNT];
        LoopQueue<uint8_t, DATASTREAM_BUFFER_SIZE, 0> databuffer;
    };
}
