#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "KeyEvent.h"
#include "RingBuffer.h"
#include "MatrixScanner.h"
#include "KeyboardProcessor.h"
#include "WiFiConfig.h"
#include "BLEConfig.h"
#include "LayoutConfig.h"
#include "ProcessManager.h"
#include "IProcessHandler.h"
#include "Stream.h"  // Your Stream implementation, e.g., ConsoleStream
#include "tcp_server_task.h"  // Include our TCP server task header
#include "SerialStream.h"
#include "wifi_init.h"
#include "wifi_common.h"
#include "TCPStream.h"

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
    &eventProcessor
};

ProcessManager processManager(processHandlers, 4);

static const char* TAG = "Main";

// Callback functions to be called from the TCP server task.
 void tcp_connection_established(int client_sock) {
    // If there's an old connection, close it.
    if (tcpStream != nullptr) {
        delete tcpStream;
        tcpStream = nullptr;
    }
    tcpStream = new TCPStream(client_sock);
    ESP_LOGI(TAG, "TCPStream created.");
    processManager.registerStream(tcpStream);
}



 void tcp_connection_closed() {
    if (tcpStream != nullptr) {
        processManager.unregisterStream(tcpStream);
        delete tcpStream;
        tcpStream = nullptr;
    }
    ESP_LOGI(TAG, "TCPStream closed.");
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



    // Main processing loop.
    while (1) {
        // Process the always-available SerialStream.
        processManager.process();

        // If a TCP connection is active, process it as well.
        // if (tcpStream != nullptr) {
        //     processManager.process(*tcpStream);
        //     // You can also check for disconnect conditions here and call tcp_connection_closed().
        // }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"
// #include "wifi_init.h"
// #include "tcp_server_task.h"
// #include "wifi_common.h"

// static const char *TAG = "Main";

// extern "C" void app_main(void)
// {
//     ESP_LOGI(TAG, "Starting Wi‑Fi initialization...");
//     wifi_init_and_start();

//     // Wait until connected.
//     ESP_LOGI(TAG, "Waiting for Wi‑Fi connection...");
//     EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
//                                              false, true, portMAX_DELAY);
//     if (bits & WIFI_CONNECTED_BIT) {
//          ESP_LOGI(TAG, "Wi‑Fi connected. Starting TCP server task.");
//          xTaskCreate(&tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
//     } else {
//          ESP_LOGW(TAG, "Wi‑Fi connection not established. Waiting for new credentials...");
//          // In a complete implementation, here you would wait for Wi‑Fi credentials
//          // to be updated via WiFiConfig (to be implemented later).
//     }

//     // Main loop: monitor Wi‑Fi connection and, if needed, restart the TCP server.
//     while (1) {
//          // Here you could monitor connection status and if lost, shut down TCP tasks
//          // and prompt for new credentials.
//          vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }
