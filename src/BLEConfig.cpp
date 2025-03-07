#include "BLEConfig.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "BLEConfig";

BLEConfig::BLEConfig() { 
    response = "bsel";
}

bool BLEConfig::validateHandshake(const std::string &cmd) {
    if (cmd == "ble?") {
        // stream->write("bsel", 4);
        ESP_LOGI(TAG, "BLE config handler selected.");
        return true;
    }
    return false;
}

ProcessStatus BLEConfig::process() {
    // For now, just consume any extra data.
    while (currentStream->available()) {
        currentStream->read();
    }
    return ProcessStatus::Complete;
}

void BLEConfig::release() {
    ESP_LOGI(TAG, "BLEConfig released.");
}