#include "LayoutConfig.h"
#include "esp_log.h"
#include <string.h>


LayoutConfig::LayoutConfig() { 
    TAG = "LayoutConfig";
    command = "layout";
    response = "kblayout";
}


ProcessStatus LayoutConfig::process() {
    // Just consume extra data.
    while (currentStream->available()) {
        currentStream->read();
    }
    return ProcessStatus::Complete;
}

void LayoutConfig::release() {
    ESP_LOGI(TAG, "LayoutConfig released.");
}