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

#define READER_BUFFER_SIZE 64
#define READER_STREAM_SIZE 512

namespace n64 {
    class Recorder : public oneline::OnelineDevice {
    public:
        Recorder();
        ~Recorder() override;

        void update() override;
        
        void handle_oneline(oneline::Port port) override;
    private:
        byte last_invalid_command;
        byte read_buffer[READER_BUFFER_SIZE] = {};
        CircularQueue<byte> reader_data = CircularQueue<byte>(READER_STREAM_SIZE);
    };
}
