#ifndef KEYEVENT_H
#define KEYEVENT_H

#include <stdint.h>

struct KeyEvent {
    uint8_t rowMask;  // Each bit corresponds to a row (active LOW assumed)
    uint8_t colData;  // Each bit corresponds to a column output.
};

#endif // KEYEVENT_H
