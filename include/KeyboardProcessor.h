#ifndef KEYBOARDPROCESSOR_H
#define KEYBOARDPROCESSOR_H

#include "IProcessHandler.h"
#include "KeyEvent.h"
#include "RingBuffer.h"
#include "Stream.h"

class KeyboardProcessor : public IProcessHandler {
   public:
    KeyboardProcessor(RingBuffer &ringBuffer);

    ProcessStatus process() override;
    void release() override;

   private:
    RingBuffer *ringBuffer;
};

#endif  // KEYBOARDPROCESSOR_H
