#include "BLEConfig.h"
#include "esp_log.h"
#include <string.h>

// const char* IProcessHandler::TAG = "BLEConfig";

BLEConfig::BLEConfig() { 
    TAG = "BLEConfig";
    command = "ble";
    response = "bsel";
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