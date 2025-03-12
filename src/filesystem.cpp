#include "filesystem.h"

#include "esp_spiffs.h"
#include "esp_log.h"

static const char *TAG = "SPIFFS";

void init_spiffs(void)
{
    ESP_LOGI(TAG, "Mounting SPIFFS filesystem");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage", // Matches the partition label in your CSV
        .max_files = 5,               // Maximum number of files that can be open at once
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount or format filesystem, error: %s", esp_err_to_name(ret));
        return;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information, error: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "SPIFFS partition size: total: %d bytes, used: %d bytes", total, used);
    }
}
