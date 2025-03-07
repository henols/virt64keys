
// #include "wifi_init.h"
// #include "wifi_common.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "esp_log.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"

#include "wifi_init.h"

#include "esp_event.h"
// #include "esp_event_base.h"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "wifi_common.h"

static const char *TAG = "wifi_init";

EventGroupHandle_t wifi_event_group = NULL;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
            esp_wifi_connect();
            ESP_LOGI(TAG, "Disconnected. Retrying connection...");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&ip_event->ip_info.ip));
    }
}

esp_err_t wifi_init_and_start(void) {
    ESP_LOGI(TAG, "Starting Wi‑Fi initialization...");
    wifi_event_group = xEventGroupCreate();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize TCP/IP network stack");
        return ret;
    }
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create default event loop");
        return ret;
    }
    ret = esp_wifi_set_default_wifi_sta_handlers();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set default handlers");
        return ret;
    }

    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    if (netif == NULL) {
        ESP_LOGE(TAG, "Failed to create default WiFi STA interface");
        return ESP_FAIL;
    }

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        },
    };

    
    ESP_ERROR_CHECK(esp_wifi_init(& (wifi_init_config_t) WIFI_INIT_CONFIG_DEFAULT()));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_LOGI(TAG, "Connecting to SSID: %s", CONFIG_WIFI_SSID);

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "Wi‑Fi initialization complete. Waiting for connection...");
    return ESP_OK;
}
