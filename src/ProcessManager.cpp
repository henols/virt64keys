#include "ProcessManager.h"
#include "esp_log.h"

static const char *TAG = "ProcessManager";

ProcessManager::ProcessManager(IProcessHandler** handlers, size_t count)
    : _handlers(handlers), _count(count), stream(nullptr) { }

bool ProcessManager::process() {
    Stream* localStream = nullptr;
    {
        // Lock the mutex only while reading the pointer.
        std::lock_guard<std::mutex> lock(streamMutex);
        localStream = stream;
    }
    if (localStream == nullptr) {
        return false;
    }

    // Now, work with localStream safely assuming that unregisterStream
    // only sets the pointer to nullptr and does not delete the object.
    if (localStream->available() < 1) return false;
    int firstByte = localStream->read();
    
    if (firstByte == '@') {
        ESP_LOGI(TAG, "ProcessManager received handshake marker.");
        std::string cmd;
        // Read the handshake command string until a delimiter is encountered.
        while (localStream->available() > 0) {
            int c = localStream->peek();
            if (c == ' ' || c == '\n' || c == '\r')
                break;
            cmd.push_back(static_cast<char>(localStream->read()));
        }
        // Optionally, consume the delimiter.
        if (localStream->available() > 0) {
            int delim = localStream->peek();
            if (delim == ' ' || delim == '\n' || delim == '\r')
                localStream->read();
        }
        ESP_LOGI(TAG, "ProcessManager received handshake command: %s", cmd.c_str());
        // Iterate over handlers and call handshake().
        for (size_t i = 0; i < _count; i++) {
            if (_handlers[i]->handshake(localStream)) {
                return true;
            }
        }
    } else if (firstByte == '#') {
        ESP_LOGI(TAG, "ProcessManager received process marker.");
        // Dispatch process marker to handlers.
        for (size_t i = 0; i < _count; i++) {
            ProcessStatus status = _handlers[i]->process();
            if (status != ProcessStatus::NotProcess) {
                return true;
            }
        }
    }
    return false;
}
