#include "RingBuffer.h"

RingBuffer::RingBuffer() : head(0), tail(0) { }

uint8_t RingBuffer::getFreeSpace() const {
    if (tail <= head) {
        return (BUFFER_SIZE - (head - tail) - 1);
    } else {
        return (tail - head - 1);
    }
}

bool RingBuffer::push(const KeyEvent &event) {
    uint8_t nextHead = (head + 1) % BUFFER_SIZE;
    if (nextHead == tail) return false;
    buffer[head] = event;
    head = nextHead;
    return true;
}

bool RingBuffer::peek(KeyEvent &event) const {
    if (head == tail) return false;
    event = buffer[tail];
    return true;
}

bool RingBuffer::pop() {
    if (head == tail) return false;
    tail = (tail + 1) % BUFFER_SIZE;
    return true;
}
