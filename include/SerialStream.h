#ifndef SERIALSTREAM_H
#define SERIALSTREAM_H

#include "Stream.h"
#include "driver/uart.h"
#include <string>

class SerialStream : public Stream {
public:
    // Constructor accepts the UART port number (we'll use UART_NUM_2)
    SerialStream(uart_port_t uart_num = UART_NUM_2);
    virtual ~SerialStream();
    virtual int available() override;
    virtual int read() override;
    virtual int peek() override;
    virtual void write(const char* data, size_t len) override;
    virtual std::string readStringUntil(char terminator) override;

private:
    uart_port_t uart_num;
    int peekBuffer; // Stores a peeked byte, if any.
    bool hasPeeked;

};

#endif // SERIALSTREAM_H
