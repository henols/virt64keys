#include "LayoutConfig.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "LayoutConfig";

LayoutConfig::LayoutConfig() { 
    response = "kblayout";
}

bool LayoutConfig::validateHandshake(const std::string &cmd) {
    if (cmd == "lay?") {
        return true;
    }
    return false;
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