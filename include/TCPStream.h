#ifndef TCPSTREAM_H
#define TCPSTREAM_H

#include "Stream.h"
#include <string>
#include <cstddef>

class TCPStream : public Stream {
public:
    // Construct TCPStream from a connected socket.
    TCPStream(int sock);
    virtual ~TCPStream();
    virtual int isConnected() override;
    virtual int available() override;
    virtual int read() override;
    virtual int read(char* dest, int len) ;
    virtual int peek() override;
    virtual void write(const char* data, size_t len) override;
    virtual std::string readStringUntil(char terminator) override;

private:
    int socket_fd;         // the underlying socket descriptor
    int peekBuffer;   // holds a peeked byte
    bool hasPeeked;
    bool connected;
    virtual int fillBuffer() ;
};

#endif // TCPSTREAM_H
