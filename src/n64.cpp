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

// REFERENCE: https://kthompson.gitlab.io/2016/07/26/n64-controller-protocol.html

#define BUFFER_SIZE 8

volatile n64::DataPacket input_buffer[BUFFER_SIZE];
volatile uint packets = 0;

namespace n64::core1 {
    void handle_packet() {
        uint timestamp = time_us_32();
        oneline::Controller controller = oneline::get_controller();
        int command = oneline::read_byte_blocking(controller);
        
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
        
        volatile DataPacket *packet = &input_buffer[packets % BUFFER_SIZE];
        uint bits = oneline::read_bytes_blocking((uint8_t*)packet->data, controller, DATA_PACKET_BUFFER, console_bytes);
        packet->timestamp = timestamp;
        packet->source = controller;
        packet->command = command;
        packet->bits = bits - 9; // The command & handover bits are counted.
        packets++;
    }
    
    uint test_controller_type = 0x05000100;
    uint test_controller_response = 0;
    
    void __time_critical_func(handle_packet_write_test)() {
        oneline::Controller controller = oneline::get_controller();
        int command = oneline::read_byte_blocking(controller);
        uint address;

        QUICK_SLEEP_US(5);
        
        switch (command) {
        case 0: // Setup Controller
        case 0xFF: // Reset Controller
            oneline::write_bytes(controller, &test_controller_type, 3);
            break;
        case 1: // Read Inputs
            oneline::write_bytes(controller, &test_controller_response, 4);
            break;
        case 2:
            address = oneline::read_byte_blocking(controller);
            address = (address << 8) | oneline::read_byte_blocking(controller);
            break;
        default:
            // Unknown commands: Discard all the data
            oneline::read_discard(controller);
            break;
        }
        pio_interrupt_clear(ONELINE_PIO, controller);
    }
    
    void record_init() {
        oneline::init();
        oneline::set_handler(&handle_packet_write_test);
        
        // CPU1 needs to spin to keep things working right.
        while (true) tight_loop_contents();
    }
}

namespace n64 {
    void record() {
        multicore_launch_core1(&core1::record_init);
        
        uint printed_packets = 0;
        uint current_packet = 0;
        
        while (true) {
            while (printed_packets == packets);
            
            volatile DataPacket *packet = &input_buffer[printed_packets % BUFFER_SIZE];
            print_int_hex(packet->timestamp);
            putchar(',');
            putchar('0' + packet->source);
            putchar(',');
            print_byte_hex(packet->command);
            putchar(',');
            print_short_hex(packet->bits);
            putchar(',');
            print_bytes_hex((uint8_t*)packet->data, packet->bits / 8);
            putchar('\n');
            printed_packets++;
        }
    }
}
