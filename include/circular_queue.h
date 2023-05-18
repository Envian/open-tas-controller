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

template <typename T>
class CircularQueue {
private:
    const int size;
    T* buffer;
    int rptr = 0, wptr = 0;
    int available = 0;
    bool underflow = false, overflow = false;

    // I keep getting burned by pass by value vs pass by reference. 
    CircularQueue(const CircularQueue& other) = delete;
    CircularQueue& operator=(const CircularQueue& other) = delete;
    CircularQueue(CircularQueue&& other) = delete;
    CircularQueue& operator=(CircularQueue&& other) = delete;
public:
    CircularQueue(int size) : size(size), buffer(new T[size]) {
        wptr = 0;
        rptr = 0;
        available = 0;
        // Do we need to clear the buffer before use?
        // for (int x = 0; x < sizeof(T) * size; x++) {
        //     *((byte*)buffer + x) = 0;
        // }
    }
    ~CircularQueue() {
        delete buffer;
    }

    T get() {
        T value = buffer[rptr];
        rptr = (rptr + 1) % size;
        available--;
        underflow |= available < 0;
        return value;
    }

    T get_blocking() {
        while (gets_avaiable() == 0) { tight_loop_contents(); }
        return get();
    }

    void add(T value) {
        buffer[wptr] = value;
        wptr = (wptr + 1) % size;
        available++;
        overflow |= available >= size;
    }
    
    void add(const T values[], int count) {
        for (int x = 0; x < count; x++) {
            buffer[(wptr + x) % size] = values[x];
        }
        wptr = (wptr + count) % size;
        available += count;
        overflow |= available >= size;
    }

    int gets_avaiable() {
        return available;
    }
    int adds_available() {
        return size - available;
    }

    bool overflowed() const { return overflow; }
    bool underflowed() const { return underflow; }
};
