#include "TCPStream.h"

#include <esp_log.h>
#include <sys/ioctl.h>

#include <string>

#include "lwip/errno.h"
#include "lwip/sockets.h"

static const char* TAG = "TCPStream";

TCPStream::TCPStream(int sock)
    : sock(sock), peekBuffer(0), hasPeeked(false), connected(true) {
    // Optionally set the socket to non-blocking mode if desired.
}

TCPStream::~TCPStream() {
    ESP_LOGI(TAG, "Closing TCPStream");
    if (sock >= 0) {
        lwip_close(sock);
    }
}

int TCPStream::isConnected() {
    return connected ? 0 : -1;
}

int TCPStream::available() {
    if (isConnected() < 0) {
        return -1;
    }
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sock, &read_fds);

    // Set timeout to zero so select() is non-blocking.
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int ret = select(sock + 1, &read_fds, NULL, NULL, &tv);
    if (ret < 0) {
        ESP_LOGE(TAG, "select() error: errno %d", errno);
        return -1;  // Signal error
    }
    if (ret == 0) {
        // No data is available
        return 0;
    }

    if (FD_ISSET(sock, &read_fds)) {
        // Data is available or the socket is closed.
        // Use recv() with MSG_PEEK and MSG_DONTWAIT to check if the connection is closed.
        uint8_t dummy;
        int n = recv(sock, &dummy, 1, MSG_PEEK | MSG_DONTWAIT);
        if (n == 0) {
            connected = false;
            // An orderly shutdown has occurred.
            return -1;
        }
        if (n < 0) {
            // If the error is EAGAIN or EWOULDBLOCK, no data is available.
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;
            } else {
                ESP_LOGE(TAG, "recv(MSG_PEEK) error: errno %d", errno);
                connected = false;
                return -1;
            }
        }
        // We know at least one byte is available.
        // (We could try to call ioctl(FIONREAD) here as a fallback if supported.)
        return 1;
    }
    return 0;
}
int TCPStream::read() {
    if (isConnected() < 0) {
        return -2;
    }
    if (hasPeeked) {
        hasPeeked = false;
        return peekBuffer;
    }

    uint8_t byte;
    int ret = recv(sock, &byte, 1, 0);
    if (ret < 0) {
        return -1;
    } else if (ret == 0) {
        connected = false;
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        ESP_LOGI(TAG, "Client disconnected");
        return -2;
    }
    // Process received data.
    return byte;
}

int TCPStream::peek() {
    if (isConnected() < 0) {
        return -1;
    }

    if (!hasPeeked) {
        uint8_t byte;
        int ret = recv(sock, &byte, 1, MSG_PEEK);
        if (ret > 0) {
            peekBuffer = byte;
            hasPeeked = true;
        } else {
            return -1;
        }
    }
    return peekBuffer;
}

void TCPStream::write(const char* data, size_t len) {
    if (isConnected() < 0) {
        return ;
    }

    send(sock, data, len, 0);
}

std::string TCPStream::readStringUntil(char terminator) {
    if (isConnected() < 0) {
        return "";
    }

    std::string result;
    while (true) {
        int c = read();
        if (c == -1) break;  // no more data
        if ((char)c == terminator) break;
        result.push_back((char)c);
    }
    return result;
}
