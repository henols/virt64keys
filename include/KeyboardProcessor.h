#ifndef KEYBOARDPROCESSOR_H
#define KEYBOARDPROCESSOR_H

#include "RingBuffer.h"
#include "KeyEvent.h"
#include "Stream.h"
#include "IProcessHandler.h"

class KeyboardProcessor : public IProcessHandler {
public:
    KeyboardProcessor(RingBuffer &ringBuffer);
    
    // For event handshake, we expect the command "cbm"
    virtual bool validateHandshake(const std::string &cmd) override;
    
    // Process event data:
    // The protocol: first byte = number of event pairs,
    // followed by eventCount * 2 bytes (rowMask, colData).
    virtual ProcessStatus process() override;
    virtual void release() override;
private:
    RingBuffer *ringBuffer;
};

#endif // KEYBOARDPROCESSOR_H
