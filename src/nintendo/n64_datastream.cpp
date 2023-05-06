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

#include "nintendo/n64_datastream.h"

#include <pico/multicore.h>
#include <hardware/irq.h>

#include "nintendo/oneline.h"
#include "io.h"

namespace n64::datastream::core1 {
    // void handle_packet(oneline::Port port) {
    //     uint timestamp = time_us_32();
    //     int command = oneline::read_byte_blocking(port);
    //
    //     uint console_bytes;
    //     switch (command) {
    //     case -1:
    //         return;
    //     case 2: // Read from game pak
    //         console_bytes = 2;
    //         break;
    //     case 3: // Write to game pak
    //         console_bytes = 34;
    //         break;
    //     default:
    //         console_bytes = 0;
    //     }
    //
    //     DataPacket *packet = &input_buffer[packets % BUFFER_SIZE];
    //     int bits = oneline::read_bytes_blocking((uint8_t*)packet->data, port, DATA_PACKET_BUFFER, console_bytes);
    //     packet->timestamp = timestamp;
    //     packet->source = port;
    //     packet->command = command;
    //     packet->bits = bits - 9;
    //     packets++;
    // }
}

namespace n64 {
    volatile uint dataRequested = 0;

    void core1_init() {
        // Sure hope this is always an n64 datastream reference...
        ((n64_Datastream*)currentDevice)->core1_loop();
    }

    n64_Datastream::n64_Datastream() {
        // Clear out controller headers
        for (uint x = 0; x < N64_CONTROLLER_COUNT; x++) {
            controllers[x].connected = false;
            for (uint y = 0; y < sizeof(controllers[x].header); y++) {
                controllers[x].header[y] = 0;
            }
        }

        // FIXME
        multicore_launch_core1(core1_init);
    }


    n64_Datastream::~n64_Datastream() {
        // TODO: Deconstructor
    }

    // Datastream format:
    // 1 byte - size of buffer
    // n bytes - Data to send to the datastream.
    bool n64_Datastream::handle_datastream() {
        uint count = io::read_blocking();
        for (uint x = 0; x < count; x++) {
            this->databuffer.add(io::read_blocking());
        }
        dataRequested -= count;
        return true;
    }

    // Controller Config Protocol:
    // 4x of the following:
    //   1 byte  - controller info (0 disconnected)
    //   3 bytes - controller header
    bool n64_Datastream::handle_controller_config() {
        for (uint x = 0; x < N64_CONTROLLER_COUNT; x++) {
            this->controllers[x].connected = !!io::read_blocking();
            for (uint n = 0; n < sizeof(this->controllers[x].header); n++) {
                this->controllers[x].header[n] = io::read_blocking();
            }
        }
        return true;
    }

    void n64_Datastream::handle_oneline(oneline::Port port) {
        ControllerConfig *controller = &controllers[port];
        if (!controller->connected) {
            return oneline::read_discard(port);
        }

        oneline::Writer writer(port);
        int command = oneline::read_byte_blocking(port);

        // Note: Can't respond too quickly, or the N64 will not register the command.
        switch (command) {
        case 0: // Identify Controller
        case 0xFF: // Reset Controller
            fast_wait_us(5);
            writer.begin_reply(3);
            writer.write(&controller->header[port * 3]);
            break;
        case 1: // Read Inputs
            fast_wait_us(5);
            writer.begin_reply(4);
            writer.write(this->databuffer.get());
            writer.write(this->databuffer.get());
            writer.write(this->databuffer.get());
            writer.write(this->databuffer.get());
            break;
        // case 2:
        //     oneline::read_byte_blocking(port);
        //     oneline::read_byte_blocking(port);
        //     fast_wait_us(6);
        //     writer.begin_reply(33);
        //     for (uint n = 0; n < 33; n++) {
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

    void n64_Datastream::core1_loop() {
        oneline::init();
        while (true) {
            int count = this->databuffer.adds_available() - dataRequested;
            if (count > 0) {
                io::write_blocking(0x80);
                io::write_blocking(count);
                dataRequested += count;
            }
        }
    }
}
