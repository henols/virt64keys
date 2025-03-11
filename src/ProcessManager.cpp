#include "ProcessManager.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <cstddef>

const char* ProcessManager::TAG = "ProcessManager";

ProcessManager::ProcessManager(IProcessHandler** handlers, size_t count)
    : _handlers(handlers), _count(count), stream(nullptr) {
    streamMutex = xSemaphoreCreateMutex();
}

bool ProcessManager::process() {
    bool res = false;
    if (stream != nullptr && xSemaphoreTake(streamMutex, 0) == pdTRUE) {
        if (stream != nullptr) {
            res = process(stream);
        }
        ESP_LOGI(TAG, "ProcessManager giving back mutex.");
        xSemaphoreGive(streamMutex);
        ESP_LOGI(TAG, "ProcessManager mutex given back.");
    }
    return res;
}

bool ProcessManager::process(Stream* localStream) {
    ESP_LOGI(TAG, "ProcessManager processing stream.");
    int avl = localStream->available();
    if (avl < 1) {
        // localStream->write("no data available\n", 18);
        // ESP_LOGI(TAG, "ProcessManager no data available %d.",avl);
        return false;
    }
    ESP_LOGI(TAG, "ProcessManager data available %d.", avl);
    int firstByte = localStream->read();

    if (firstByte == '@') {
        ESP_LOGI(TAG, "ProcessManager received handshake marker.");
        // std::string cmd = stream->readStringUntil(' ');

        // for (size_t i = 0; i < _count; i++) {
        //     if (_handlers[i]->handshake(cmd, localStream)) {
        //         return true;
        //     }
        // }
    } else if (firstByte == '#') {
        ESP_LOGI(TAG, "ProcessManager received process marker.");
        // for (size_t i = 0; i < _count; i++) {
        //     ProcessStatus status = _handlers[i]->process();
        //     if (status != ProcessStatus::NotProcess) {
        //         return true;
        //     }
        // }
    }
    return false;
}
