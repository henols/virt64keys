#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <cstddef>
// #include <mutex>  // Include mutex header
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "IProcessHandler.h"
#include "Stream.h"
#include <esp_log.h>


class ProcessManager {
   public:
    // Construct with an array of IProcessHandler pointers and their count.
    ProcessManager(IProcessHandler **handlers, size_t count);

    // Process the stream: first byte decides if it's a handshake.
    // If the first byte is '@', read the command string and call handshake().
    // Otherwise, dispatch the data via process().
    // Returns true if a handler handled data.
    virtual bool process();
    virtual bool process(Stream *stream);

    // Use the mutex to protect changes to stream.
    bool registerStream(Stream *stream) {
        xSemaphoreTake(streamMutex, portMAX_DELAY);
        ESP_LOGI(TAG, "Registering stream.");
        this->stream = stream;
        xSemaphoreGive(streamMutex);
        return true;
    }

    bool unregisterStream(Stream *stream) {
        xSemaphoreTake(streamMutex, portMAX_DELAY);
        bool res = this->stream == stream;
        ESP_LOGI(TAG, "Unregistering stream: %s", res ? "true" : "false");
        if (res) {
            this->stream = nullptr;
            // return true;
        }
        xSemaphoreGive(streamMutex);
        return res;
    }

   private:
    IProcessHandler **_handlers;
    size_t _count;
    Stream *stream;
    // mutable std::mutex streamMutex; // Mutex to guard stream access
    SemaphoreHandle_t streamMutex = NULL;
    static const char *TAG ;
};

#endif  // PROCESSMANAGER_H
