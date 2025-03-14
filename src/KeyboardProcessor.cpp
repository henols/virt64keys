#include "KeyboardProcessor.h"

#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



KeyboardProcessor::KeyboardProcessor(RingBuffer &rb)
: ringBuffer(&rb){
        TAG = "KeyboardProcessor";
        command = "cbm";
     response = "c64";
}


ProcessStatus KeyboardProcessor::process() {
    if (!eventHandshakeDone || currentStream == nullptr) {
        return ProcessStatus::NotProcess;
    }
    // Process event data:
    // Protocol: first byte = event count, followed by eventCount * 2 bytes.
    if (currentStream->available() < 1) return ProcessStatus::NotProcess;
    int eventCount = currentStream->read();
    uint8_t capacity = ringBuffer->getFreeSpace();
    if (eventCount > capacity) eventCount = capacity;
    while (currentStream->available() < (size_t)eventCount * 2) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    for (int i = 0; i < eventCount; i++) {
        KeyEvent event;
        event.rowMask = currentStream->read();
        event.colData = currentStream->read();
        ringBuffer->push(event);
    }
    return ProcessStatus::Complete;
}

void KeyboardProcessor::release() {
    ESP_LOGI(TAG, "KeyboardProcessor released.");
}