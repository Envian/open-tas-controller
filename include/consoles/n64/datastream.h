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
#include "consoles/n64/model.h"

#include "consoles/common/oneline.h"
#include "circular_queue.h"

#define DATASTREAM_BUFFER_SIZE 128
#define RAW_DATA_STREAM_SIZE 512
#define N64_CONTROLLER_COUNT 4

namespace n64 {
    class Datastream : public BaseDevice, public oneline::OnelineHandler {
    public:
        Datastream();
        ~Datastream() override;

        void update() override;
        
        void handle_datastream() override;
        void handle_controller_config() override;
        void handle_oneline(oneline::Port port) override;
    private:
        bool pending_data = false;
        uint last_event = 0;
        oneline::Port last_port;
        byte last_input[4];
        ControllerConfig controllers[N64_CONTROLLER_COUNT];
        CircularQueue<byte> databuffer = CircularQueue<byte>(DATASTREAM_BUFFER_SIZE);
    };
}
