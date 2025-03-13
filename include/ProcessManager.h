#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <cstddef>

#include "IProcessHandler.h"
#include "Stream.h"

class ProcessManager {
   private:
    IProcessHandler **_handlers;
    size_t _count;
    Stream *stream;
    SemaphoreHandle_t streamMutex = NULL;
    bool process(Stream *stream);
    static constexpr const char *TAG = "ProcessManager";

   public:
    ProcessManager(IProcessHandler **handlers, size_t count);

    bool process();

    bool registerStream(Stream *stream);
    bool unregisterStream(Stream *stream);
};

#endif  // PROCESSMANAGER_H

