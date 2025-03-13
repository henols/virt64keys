#include "SerialStream.h"

#include <cstring>
#include <string>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"

static const char* TAG = "SerialStream";

SerialStream::SerialStream(uart_port_t uart_num)
    : uart_num(uart_num), peekBuffer(0), hasPeeked(false){
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set RX pin to GPIO12 and TX pin to GPIO14; no RTS/CTS pins used.
    ESP_ERROR_CHECK(uart_set_pin(uart_num, GPIO_NUM_14, GPIO_NUM_12, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Install UART driver with a buffer size of 1024 bytes.
    ESP_ERROR_CHECK(uart_driver_install(uart_num, 1024, 0, 0, NULL, 0));
    
    connected = true;
}

SerialStream::~SerialStream() {
    uart_driver_delete(uart_num);
}

int SerialStream::available() {
    size_t rx_bytes = 0;
    uart_get_buffered_data_len(uart_num, &rx_bytes);
    return (int)rx_bytes;
}

int SerialStream::read() {
    uint8_t byte;
    if (hasPeeked) {
        hasPeeked = false;
        return peekBuffer;
    }
    int len = uart_read_bytes(uart_num, &byte, 1, 10 / portTICK_PERIOD_MS);
    if (len > 0) {
        return byte;
    }
    return -1;
}

int SerialStream::peek() {
    if (!hasPeeked) {
        int b = read();
        if (b != -1) {
            peekBuffer = b;
            hasPeeked = true;
        }
    }
    return hasPeeked ? peekBuffer : -1;
}

void SerialStream::write(const char* data, size_t len) {
    uart_write_bytes(uart_num, data, len);
}

std::string SerialStream::readStringUntil(char terminator) {
    std::string result;
    while (true) {
        int c = read();
        if (c == -1) break;  // No data available.
        if ((char)c == terminator) break;
        result.push_back((char)c);
    }
    return result;
}
