#ifndef IPROCESSHANDLER_H
#define IPROCESSHANDLER_H

#include <string>

#include "Stream.h"
#include "esp_log.h"

// ProcessStatus indicates the state after processing data.
enum class ProcessStatus {
    NotProcess,  // The incoming data does not belong to this handler.
    InProgress,  // Processing is in progress (more data needed).
    Complete     // Processing completed successfully.
};

class IProcessHandler {
   public:
    bool handshake(const std::string &cmd, Stream *stream) {
        bool res = false;
        ESP_LOGI(TAG, "Validating handshake command: %s", cmd.c_str());
        if (cmd == command) {
            ESP_LOGI(TAG, "Handshake successful %s -> %s", command.c_str(), response.c_str());

            currentStream = stream;
            currentStream->write(response.c_str(), response.size());
            eventHandshakeDone = true;
            res = true;
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
    std::string command;
    std::string response;
    Stream *currentStream;
    bool eventHandshakeDone = false;
    const char *TAG;

};

#endif  // IPROCESSHANDLER_H
