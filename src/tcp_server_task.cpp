#include "tcp_server_task.h"

#include <sys/ioctl.h>

#include "TCPStream.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/errno.h"
#include "lwip/sockets.h"

// Declare the external callback functions (implemented elsewhere, e.g., in main.c)
extern void tcp_connection_established(int client_sock);
extern void tcp_connection_closed(void);

extern TCPStream *tcpStream;

static const char *TAG = "TCPServer";

int create_server_socket(uint16_t port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create socket: errno %d", errno);
        return -1;
    }

    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Failed to bind socket: errno %d", errno);
        lwip_close(sock);
        return -1;
    }

    if (listen(sock, 1) < 0) {
        ESP_LOGE(TAG, "Failed to listen on socket: errno %d", errno);
        lwip_close(sock);
        return -1;
    }
    ESP_LOGI(TAG, "TCP server listening on port %d", port);
    return sock;
}

void tcp_server_task(void *pvParameters) {
    int server_sock = create_server_socket(6464);
    if (server_sock < 0) {
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            ESP_LOGE(TAG, "Accept failed: errno %d", errno);
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        ESP_LOGI(TAG, "Client connected on port 6464");
        // Notify the main application that a client has connected.
        tcp_connection_established(client_sock);
        while (1) {
            if (tcpStream && tcpStream->isConnected() < 0) {
                tcp_connection_closed();
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        lwip_close(client_sock);
    }

    lwip_close(server_sock);
    vTaskDelete(NULL);
}
