#ifndef MATRIXSCANNER_H
#define MATRIXSCANNER_H

#include "RingBuffer.h"
#include "KeyEvent.h"
#include <stdint.h>
#include "esp_attr.h"  // Ensure IRAM_ATTR is defined

// If IRAM_ATTR is not defined for some reason, define it:
#ifndef IRAM_ATTR
#define IRAM_ATTR __attribute__((section(".iram.text")))
#endif

class MatrixScanner {
public:
    MatrixScanner(RingBuffer &ringBuffer, const uint8_t* rowPins, uint8_t rowCount, const uint8_t* colPins, uint8_t colCount);
    void begin();
    void handleInterrupt();
    uint8_t getLastRowFromMask(uint8_t mask);
    
private:
    RingBuffer &ringBuffer;
    const uint8_t* rowPins;
    uint8_t rowCount;
    const uint8_t* colPins;
    uint8_t colCount;
    
    void updateOutputs(const KeyEvent &event, int triggeredRow);
    
    static MatrixScanner* instance;
    // Note: Make sure to include "esp_attr.h" so IRAM_ATTR is defined.
    static void IRAM_ATTR isrHandler(void* arg);
};

#endif // MATRIXSCANNER_H
