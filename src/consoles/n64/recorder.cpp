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

#include "consoles/n64/recorder.h"


#include "helpers.h"
#include "consoles/common/oneline.h"
#include "io.h"
#include "labels.h"

namespace n64 {
    Recorder::Recorder() {
        oneline::init(this);
        io::Info(labels::INFO_DEVICE_INIT).write(labels::CONSOLE_N64).write(labels::DEVICE_TYPE_DATASTREAM);
    }

    Recorder::~Recorder() {
        oneline::uninit();
    }

    void Recorder::update() {
        while (this->reader_data.gets_avaiable() > 0) {
            byte port = this->reader_data.get();
            byte size = this->reader_data.get();
            byte request_size = this->reader_data.get();

            io::CommandWriter(commands::device::RAW_DATA)
                .write_byte(port)
                .write_byte(size)
                .write_byte(request_size)
                .write_bytes(&this->reader_data, size);
        }

        if (this->reader_data.overflowed()) {
            io::Error(labels::ERROR_BUFFER_OVERFLOW).write(__FILE__).send();
        }

        if (this->reader_data.underflowed()) {
            io::Error(labels::ERROR_BUFFER_UNDERFLOW).write(__FILE__).send();
        }

        if (this->last_invalid_command) {
            io::Warn(labels::WARN_UNKNOWN_CONSOLE_CMD).write_byte(this->last_invalid_command).send();
            this->last_invalid_command = 0;
        }
    }
    
    void Recorder::handle_oneline(oneline::Port port) {
        int command = oneline::read_byte_blocking(port);

        if (command == -1) {
            return;
        }

        int additional_request_bytes = 0;
        int response_bytes = 0;

        switch (command) {
        case 0x00: // Identify Controller
        case 0xFF: // Reset Controller
            response_bytes = 3;
            break;
        case 0x01: // Read Inputs
            response_bytes = 4;
            break;
        default:
            // Unknown commands. While most systems could just dump data,
            // we need to know where the handoff bit is, otherwise the 
            // controller response will be bit shifted by one. For testing,
            // we could still write the data, but its not useful to a user.
            last_invalid_command = command;
            return;
        }

        // Read the remaining data.
        int actual_data_count = oneline::read_bytes_blocking(this->read_buffer, port, 
            additional_request_bytes + response_bytes, additional_request_bytes);


        //this->reader_data.add(commands::device::RAW_DATA);
        this->reader_data.add(port);
        this->reader_data.add(actual_data_count + 1);
        this->reader_data.add(additional_request_bytes + 1);
        this->reader_data.add(command);
        this->reader_data.add(this->read_buffer, actual_data_count);
    }
}
