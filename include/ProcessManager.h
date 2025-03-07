#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <cstddef>
#include <mutex>  // Include mutex header

#include "IProcessHandler.h"
#include "Stream.h"

class ProcessManager {
   public:
    // Construct with an array of IProcessHandler pointers and their count.
    ProcessManager(IProcessHandler **handlers, size_t count);

    // Process the stream: first byte decides if it's a handshake.
    // If the first byte is '@', read the command string and call handshake().
    // Otherwise, dispatch the data via process().
    // Returns true if a handler handled data.
    bool process();

    // Use the mutex to protect changes to stream.
    bool registerStream(Stream *stream) {
        std::lock_guard<std::mutex> lock(streamMutex);
        this->stream = stream;
        return true;
    }

    bool unregisterStream(Stream *stream) {
        std::lock_guard<std::mutex> lock(streamMutex);
        if (stream == this->stream) {
            this->stream = nullptr;
            return true;
        }
        return false;
    }

   private:
    IProcessHandler **_handlers;
    size_t _count;
    Stream *stream;
    mutable std::mutex streamMutex; // Mutex to guard stream access
};

#endif  // PROCESSMANAGER_H
