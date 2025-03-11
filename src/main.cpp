#include "BLEConfig.h"
#include "IProcessHandler.h"
#include "KeyEvent.h"
#include "KeyboardProcessor.h"
#include "LayoutConfig.h"
#include "MatrixScanner.h"
#include "ProcessManager.h"
#include "RingBuffer.h"
#include "SerialStream.h"
#include "Stream.h"  // Your Stream implementation, e.g., ConsoleStream
#include "TCPStream.h"
#include "WiFiConfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tcp_server_task.h"  // Include our TCP server task header
#include "wifi_common.h"
#include "wifi_init.h"

// Define the matrix pins (adjust as needed).
const uint8_t rowPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};
const uint8_t colPins[8] = {10, 11, 12, 13, 14, 15, 16, 17};

// Global SerialStream: always available.
SerialStream serialStream(UART_NUM_2);

// Global pointer for the TCPStream; initially nullptr.
TCPStream* tcpStream = nullptr;

RingBuffer ringBuffer;
MatrixScanner matrixScanner(ringBuffer, rowPins, 8, colPins, 8);
KeyboardProcessor eventProcessor(ringBuffer);

// Create process handlers.
WiFiConfig wifiConfig;
BLEConfig bleConfig;
LayoutConfig layoutConfig;
// Note: EventProcessor is also an IProcessHandler.
IProcessHandler* processHandlers[] = {
    &wifiConfig,
    &bleConfig,
    &layoutConfig,
    &eventProcessor};

ProcessManager processManager(processHandlers, 4);

static const char* TAG = "Main";
void tcp_connection_established(int client_sock);
void tcp_connection_closed();

// Callback functions to be called from the TCP server task.
void tcp_connection_established(int client_sock) {
    // If there's an old connection, close it.
    ESP_LOGI(TAG, "Establishing TCP connection.");
    tcp_connection_closed();
    tcpStream = new TCPStream(client_sock);
    processManager.registerStream(tcpStream);
}

void tcp_connection_closed() {
    if (tcpStream != nullptr) {
        ESP_LOGI(TAG, "Closing TCP connection.");
        processManager.unregisterStream(tcpStream);
        delete tcpStream;
        tcpStream = nullptr;
    }
}

void process_manager_task(void* pvParameters) {
    while (1) {
        processManager.process();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Application started");
    // Wait for Wi-Fi connection before starting the TCP server.
    //    matrixScanner.begin();
    ESP_LOGI(TAG, "Starting Wi‑Fi initialization...");
    wifi_init_and_start();

    // Wait until connected.
    ESP_LOGI(TAG, "Waiting for Wi‑Fi connection...");
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
                                           false, true, portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Wi‑Fi connected. Starting TCP server task.");
        xTaskCreate(&tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
    } else {
        ESP_LOGW(TAG, "Wi‑Fi connection not established. Waiting for new credentials...");
        // In a complete implementation, here you would wait for Wi‑Fi credentials
        // to be updated via WiFiConfig (to be implemented later).
    }

        xTaskCreate(&process_manager_task,  "process_manager", 4096, NULL, 5, NULL);
}
