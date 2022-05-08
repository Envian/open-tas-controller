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

template <typename T, int SIZE>
class LoopQueue {
private:
    T buffer[SIZE];
    uint rptr;
    uint wptr;
public:
    LoopQueue() {
        wptr = 0;
        rptr = 0;
    }
    T get() {
        T value = buffer[rptr];
        rptr = (rptr + 1) % SIZE;
        return value;
    }
    void add(T value) {
        buffer[wptr] = value;
        wptr = (wptr + 1) % SIZE;
    }
    uint gets_avaiable() {
        return (wptr < rptr) ? (wptr - rptr + SIZE) : (wptr - rptr);
    }
    uint adds_available() {
        return (rptr <= wptr) ? (rptr - wptr + SIZE) : (rptr - wptr);
    }
};
