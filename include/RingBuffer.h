#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "KeyEvent.h"
#include <stdint.h>

class RingBuffer {
public:
    RingBuffer();
    bool push(const KeyEvent &event);
    bool peek(KeyEvent &event) const;
    bool pop();
    uint8_t getFreeSpace() const;
    
private:
    static const uint8_t BUFFER_SIZE = 16;
    KeyEvent buffer[BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
};

#endif // RINGBUFFER_H
