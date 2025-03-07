#include "WiFiConfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "WiFiConfig";



WiFiConfig::WiFiConfig() { 
    response = "net";
}

bool WiFiConfig::validateHandshake(const std::string &cmd) {
    if (cmd == "wifi") {
        ESP_LOGI(TAG, "WiFi config handler selected.");
        return true;
    }
    return false;
}

ProcessStatus WiFiConfig::process() {
    // Wait for additional configuration data (expect '#' then config string)
    if (currentStream->available() < 1) return ProcessStatus::InProgress;
    if (currentStream->peek() != '#') return ProcessStatus::InProgress;
    currentStream->read();  // Consume the '#' marker.
    
    std::string config = currentStream->readStringUntil('\n'); // Assume this returns std::string.
    if (config.empty()) return ProcessStatus::InProgress;
    
    size_t colonPos = config.find(':');
    if (colonPos == std::string::npos) {
        ESP_LOGI(TAG, "Invalid WiFi configuration format.");
        return ProcessStatus::Complete;
    }
    
    std::string ssid = config.substr(0, colonPos);
    std::string password = config.substr(colonPos + 1);
    
    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid.c_str());
    // Call your WiFi connect routine here.
    vTaskDelay(pdMS_TO_TICKS(1000));  // Simulate delay.
    
    ESP_LOGI(TAG, "WiFi connected. IP: 192.168.1.100");
    currentStream->write("net", 3);
    return ProcessStatus::Complete;
}

void WiFiConfig::release() {
    ESP_LOGI(TAG, "WiFiConfig released.");
}