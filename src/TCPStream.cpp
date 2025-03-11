#include "TCPStream.h"

#include <errno.h>
#include <esp_log.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>  // for close()

#include <algorithm>  // for std::min
#include <cstring>    // for memcpy if needed
#include <string>
#include <vector>

#include "lwip/errno.h"
#include "lwip/sockets.h"

static const char* TAG = "TCPStream";
std::vector<char> buffer;

TCPStream::TCPStream(int socket_fd)
    : socket_fd(socket_fd), peekBuffer(0), hasPeeked(false), connected(true) {
    // Optionally set the socket to non-blocking mode if desired.
    // int flags = fcntl(socket_fd, F_GETFL, 0);
    // // Set non-blocking flag (O_NONBLOCK)
    // fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

TCPStream::~TCPStream() {
    ESP_LOGI(TAG, "Closing TCPStream");
    if (socket_fd >= 0) {
        lwip_close(socket_fd);
    }
}

int TCPStream::isConnected() {
    return connected ? 0 : -1;
}

int TCPStream::available() {
    return fillBuffer();
}

int TCPStream::read() {
    int byte = peek();
    if (byte < 0) {
        return byte;
    }
    if (byte >= 0) {
        buffer.erase(buffer.begin());
    }
    return byte;
}

// Overloaded read method that reads up to len bytes into the provided array.
// Returns the number of bytes read, or -1 if the socket is broken.
int TCPStream::read(char* dest, int len) {
    if (peek() < 0) {
        return -1;
    }
    int bytesToRead = std::min(len, static_cast<int>(buffer.size()));
    // Copy the bytes from the internal buffer to the destination array.
    std::copy(buffer.begin(), buffer.begin() + bytesToRead, dest);
    // Remove the bytes that were read.
    buffer.erase(buffer.begin(), buffer.begin() + bytesToRead);
    return bytesToRead;
}

int TCPStream::peek() {
    int n = fillBuffer();
    if (n < 1) {
        return -1;
    }
    return static_cast<unsigned char>(buffer.front());
}

void TCPStream::write(const char* data, size_t len) {
    if (isConnected() < 0) {
        return;
    }

    send(socket_fd, data, len, 0);
}

std::string TCPStream::readStringUntil(char terminator) {
    if (isConnected() < 0) {
        return "";
    }
    std::string cmd;

    while (available() > 0) {
        int c = read();
        if (c < 0) break;  // No data available.
        if ((char)c == terminator) break;
        if ( c == '\n' || c == '\r') {
            break;
        }
        cmd.push_back(static_cast<char>(c));
        ESP_LOGI(TAG, "Read: %c", c);        
    }
    return cmd;
}

// Continuously reads from the socket (non-blocking) and appends to the internal buffer.
int TCPStream::fillBuffer() {
    if (buffer.size() > 0) {
        return buffer.size();
    }
    if (isConnected() < 0) {
        return -2;
    }
    char tmp[4096];
    // Continue reading until no more data is available.
    while (true) {
        ssize_t n = recv(socket_fd, tmp, sizeof(tmp), MSG_DONTWAIT);
        if (n > 0) {
            // Append the read bytes to the buffer.
            buffer.insert(buffer.end(), tmp, tmp + n);
            ESP_LOGI(TAG, "Read %d bytes, %s", n, buffer.data());
        } else if (n == 0) {
            // Socket closed gracefully.
            ESP_LOGE(TAG, "Error reading from socket: %s", strerror(errno));
            connected = false;
            break;
        } else {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No more data available right now.
                break;
            } else {
                // An error occurred.
                ESP_LOGE(TAG, "Error reading from socket: %s", strerror(errno));
                connected = false;
                break;
            }
        }
    }
    if (isConnected() < 0) {
        ESP_LOGE(TAG, "Error reading from socket: %s", strerror(errno));
        return -2;
    }
    ESP_LOGI(TAG, "Buffer size: %d", buffer.size());
    return buffer.size();
}