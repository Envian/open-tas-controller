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

#include "n64.h"

#include <stdio.h>
#include <pico/multicore.h>
#include <hardware/irq.h>

#include "helpers.h"
#include "oneline.h"
#include "loop_queue.h"

#define BUFFER_SIZE 8

n64::DataPacket input_buffer[BUFFER_SIZE];
uint packets = 0;

uint32_t controller_id[4];
uint8_t controller_id_buffers[4*3] = {
    0x05, 0x00, 0x02,
    0x05, 0x00, 0x02,
    0x05, 0x00, 0x02,
    0x05, 0x00, 0x02,
};

LoopQueue<uint8_t, 128> datastream = LoopQueue<uint8_t, 128>();

namespace n64::core1 {
    void handle_packet(oneline::Port port) {
        uint timestamp = time_us_32();
        int command = oneline::read_byte_blocking(port);

        uint console_bytes;
        switch (command) {
        case -1:
            return;
        case 2: // Read from game pak
            console_bytes = 2;
            break;
        case 3: // Write to game pak
            console_bytes = 34;
            break;
        default:
            console_bytes = 0;
        }

        DataPacket *packet = &input_buffer[packets % BUFFER_SIZE];
        int bits = oneline::read_bytes_blocking((uint8_t*)packet->data, port, DATA_PACKET_BUFFER, console_bytes);
        packet->timestamp = timestamp;
        packet->source = port;
        packet->command = command;
        packet->bits = bits - 9;
        packets++;
    }

    void handler_datastream(oneline::Port port) {
        // uint32_t identifier = controller_id[(uint)port];
        // if (!identifier) {
        //     return oneline::read_discard(port);
        // }

        int command = oneline::read_byte_blocking(port);
        oneline::Writer writer(port);

        switch (command) {
        case 0: // Identify Controller
        case 0xFF: // Reset Controller
            fast_wait_us(5);
            writer.begin_reply(3);
            writer.write(&controller_id_buffers[port * 3]);
            break;
        case 1: // Read Inputs
            fast_wait_us(5);
            writer.begin_reply(4);
            writer.write(datastream.get());
            writer.write(datastream.get());
            writer.write(datastream.get());
            writer.write(datastream.get());
            break;
        case 2:
            oneline::read_byte_blocking(port);
            oneline::read_byte_blocking(port);
            fast_wait_us(6);
            writer.begin_reply(33);
            for (uint n = 0; n < 33; n++) {
                writer.write(datastream.get());
            }
            break;
        case 3:
            for (int x = 0; x < 34; x++) {
                oneline::read_byte_blocking(port);
            }

            fast_wait_us(6);
            writer.begin_reply(1);
            writer.write_zeros();
            break;
        default:
            // Unknown commands: Discard all the data
            oneline::read_discard(port);
            break;
        }
    }

    void datastream_init() {
        oneline::init();
        oneline::set_handler(&handler_datastream);

        // if CPU1 doesn't spin, then time_us_32 does not work.
        while (true) tight_loop_contents();
    }
}

namespace n64 {
    void playback_datastream() {
        multicore_launch_core1(&core1::datastream_init);


    }
}
