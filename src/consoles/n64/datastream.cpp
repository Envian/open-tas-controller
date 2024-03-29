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

#include "consoles/n64/datastream.h"

#include <pico/multicore.h>

#include "helpers.h"
#include "consoles/common/oneline.h"
#include "io.h"

#ifdef LED_SHOWS_DATASTREAM_STATUS
#define DATASTREAM_REQUEST_PENDING() LED_ON()
#define DATASTREAM_REQUEST_FILLED() LED_OFF()
#else
#define DATASTREAM_REQUEST_PENDING() 
#define DATASTREAM_REQUEST_FILLED() 
#endif

namespace n64 {
    Datastream::Datastream() {
        // Clear out controller headers
        for (int x = 0; x < N64_CONTROLLER_COUNT; x++) {
            controllers[x].connected = false;
            for (int y = 0; y < (int)sizeof(controllers[x].header); y++) {
                controllers[x].header[y] = 0;
            }
        }

        oneline::init(this);
        io::Info(labels::INFO_DEVICE_INIT).write(labels::CONSOLE_N64).write(labels::DEVICE_TYPE_DATASTREAM);
    }

    Datastream::~Datastream() {
        oneline::uninit();
    }

    void Datastream::update() {
        if (!this->pending_data && this->databuffer.adds_available()) {
            io::CommandWriter(commands::device::DATASTREAM_REQUEST)
                .write_byte(this->databuffer.adds_available());
            this->pending_data = true;
            DATASTREAM_REQUEST_PENDING();
        }
    }

    // Datastream format:
    // 1 byte - size of buffer
    // n bytes - Data to send to the datastream.
    void Datastream::handle_datastream() {
        int count = io::read_blocking();
        for (int x = 0; x < count; x++) {
            this->databuffer.add(io::read_blocking());
        }
        this->pending_data = false;
        DATASTREAM_REQUEST_FILLED();
    }

    // Controller Config Protocol:
    // 4x of the following:
    //   1 byte  - controller info (0 disconnected)
    //   3 bytes - controller header
    void Datastream::handle_controller_config() {
        for (int x = 0; x < N64_CONTROLLER_COUNT; x++) {
            this->controllers[x].connected = !!io::read_blocking();
            for (int n = 0; n < (int)sizeof(this->controllers[x].header); n++) {
                this->controllers[x].header[n] = io::read_blocking();
            }
        }

        io::Debug(labels::DEBUG_PORT_INFO)
            .write_byte(1)
            .write_byte(controllers[0].connected)
            .write_bytes(controllers[0].header, sizeof(controllers[0].header));
        io::Debug(labels::DEBUG_PORT_INFO)
            .write_byte(1)
            .write_byte(controllers[1].connected)
            .write_bytes(controllers[1].header, sizeof(controllers[1].header));
        io::Debug(labels::DEBUG_PORT_INFO)
            .write_byte(1)
            .write_byte(controllers[2].connected)
            .write_bytes(controllers[2].header, sizeof(controllers[2].header));
        io::Debug(labels::DEBUG_PORT_INFO)
            .write_byte(1)
            .write_byte(controllers[3].connected)
            .write_bytes(controllers[3].header, sizeof(controllers[3].header));
    }

    void Datastream::handle_oneline(oneline::Port port) {
        ControllerConfig *controller = &controllers[port];
        if (!controller->connected) {
            return oneline::read_discard(port);
        }

        int command = oneline::read_byte_blocking(port);

        // Note: Can't respond too quickly, or the N64 will not register the command.
        switch (command) {
        case 0: // Identify Controller
        case 0xFF: // Reset Controller
            fast_wait_us(5);
            oneline::Writer(port, 3)
                .write(&controller->header[port * 3]);
            break;
        case 1: // Read Inputs
            fast_wait_us(5);
            this->last_input[0] = this->databuffer.get();
            this->last_input[1] = this->databuffer.get();
            this->last_input[2] = this->databuffer.get();
            this->last_input[3] = this->databuffer.get(); 

            oneline::Writer(port, 4)
                .write(this->last_input);

            this->last_port = port;
            this->last_event++;
            break;
        // case 2:
        //     oneline::read_byte_blocking(port);
        //     oneline::read_byte_blocking(port);
        //     fast_wait_us(6);
        //     writer.begin_reply(33);
        //     for (int n = 0; n < 33; n++) {
        //         writer.write(state->buffer.get());
        //     }
        //     break;
        // case 3:
        //     for (int x = 0; x < 34; x++) {
        //         oneline::read_byte_blocking(port);
        //     }
        //
        //     fast_wait_us(6);
        //     writer.begin_reply(1);
        //     writer.write_zeros();
        //     break;
        default:
            // Unknown commands: Discard all the data
            oneline::read_discard(port);
            break;
        }
    }
}
