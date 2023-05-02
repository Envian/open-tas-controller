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
#include "loop_queue.h"
#include "io.h"

#define BUFFER_SIZE 128
#define MAX_REQUEST 1

struct State {
    LoopQueue<volatile uint8_t, BUFFER_SIZE> buffer = LoopQueue<volatile uint8_t, BUFFER_SIZE>();
    uint8_t controllers[4*3] = {};
};

State *state;

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

    void handler_datastream(oneline::Port port) {
        uint32_t identifier = state->controllers[(port * 3) + 2];
        if (!identifier) {
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
            writer.write(&state->controllers[port * 3]);
            break;
        case 1: // Read Inputs
            fast_wait_us(5);
            writer.begin_reply(4);
            writer.write(state->buffer.get());
            writer.write(state->buffer.get());
            writer.write(state->buffer.get());
            writer.write(state->buffer.get());
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

    void datastream_init() {
        oneline::init();
        oneline::set_handler(&handler_datastream);
    }
}

namespace n64::datastream {
    void playback() {
        state = new State();
        stdio_init_all();

        for (uint x = 0; x < 12; x++) {
            state->controllers[x] = io::read_blocking();
        }

        io::write_blocking('t');
        io::write_blocking('h');
        io::write_blocking('a');
        io::write_blocking('n');
        io::write_blocking('k');
        io::write_blocking('s');
        io::write_blocking('\n');

        // multicore_launch_core1(&core1::datastream_init);
        core1::datastream_init();
        
        
        while (true) {
            while (state->buffer.adds_available() == 0) { tight_loop_contents(); }
            
            // Tell PC how many bytes it can handle
            io::write_blocking(MIN(state->buffer.adds_available(), MAX_REQUEST));
            //io::flush();


            // PC responds with the number of bytes, and streams them all.
            uint count = io::read_blocking();
            for (uint x = 0; x < count; x++) {
                // Double ensure we don't overflow
                while (state->buffer.adds_available() == 0) { tight_loop_contents(); }
                state->buffer.add(io::read_blocking());
            }
        }
    }
}
