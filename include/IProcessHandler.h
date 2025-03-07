#ifndef IPROCESSHANDLER_H
#define IPROCESSHANDLER_H

#include <string>

#include "Stream.h"  // Your minimal Stream interface



// ProcessStatus indicates the state after processing data.
enum class ProcessStatus {
    NotProcess,  // The incoming data does not belong to this handler.
    InProgress,  // Processing is in progress (more data needed).
    Complete     // Processing completed successfully.
};

class IProcessHandler {
   public:
    // handshake() is called with a command string extracted from the stream.
    // If the command matches this handler, the handler should send a selection
    // response using the stream and return true; otherwise, false.
    bool handshake(Stream *stream) {
        if (stream->available() < 1) {
            return false;
        }
        std::string cmd = stream->readStringUntil(' ');
        bool res = validateHandshake(cmd);
        if (res) {
            currentStream = stream;
        }
        return res;
    }
    // Called repeatedly after a handler is selected to process incoming data.
    virtual ProcessStatus process() = 0;

    // Called when the stream is no longer valid, so the processor resets itself.
    void releaseHandler() {
        // Reset internal state.
        eventHandshakeDone = false;
        currentStream = nullptr;
        release();
    }
    virtual void release() = 0;

    virtual ~IProcessHandler() {}
    private:

   protected:
    Stream *currentStream;
    virtual bool validateHandshake(const std::string &cmd) = 0;
    std::string response;
    bool eventHandshakeDone = false;
};

#endif  // IPROCESSHANDLER_H
