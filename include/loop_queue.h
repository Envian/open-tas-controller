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

template <typename T, int SIZE, T underflowValue>
class LoopQueue {
private:
    T buffer[SIZE];
    uint rptr = 0, wptr = 0;
    volatile int available = 0;
    bool underflow = false, overflow = false;
public:
    LoopQueue() {
        wptr = 0;
        rptr = 0;
        available = 0;
        for (uint x = 0; x < sizeof(buffer); x++) {
            *((uint8_t*)buffer + x) = 0;
        }
    }

    volatile T get() {
        T value = buffer[rptr];
        rptr = (rptr + 1) % SIZE;
        available--;
        underflow |= available < 0;
        return value;
    }

    volatile void add(T value) {
        buffer[wptr] = value;
        wptr = (wptr + 1) % SIZE;
        available++;
        overflow |= available >= SIZE;
    }
    
    volatile void add(const T values[], uint count) {
        for (uint x = 0; x < count; x++) {
            buffer[(wptr + x) % SIZE] = values[x];
        }
        wptr = (wptr + count) % SIZE;
        available += count;
        overflow |= available >= SIZE;
    }

    volatile uint gets_avaiable() {
        return available;
    }
    volatile uint adds_available() {
        return SIZE - available;
    }
};
