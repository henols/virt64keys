#ifndef STREAM_H
#define STREAM_H

#include <string>
#include <cstddef>

class Stream {
public:
    virtual ~Stream() {}

    // Returns the number of bytes available to read.
    virtual int available() = 0;

    // Reads and returns a single byte, or -1 if none available.
    virtual int read() = 0;

    // Returns the next byte without consuming it, or -1 if none available.
    virtual int peek() = 0;
    
     int isConnected() {
        return connected ? 0 : -1;

     }

    // Writes 'len' bytes from data.
    virtual void write(const char* data, size_t len) = 0;

    // Reads characters until the terminator is reached,
    // returns the read string (without the terminator).
    virtual std::string readStringUntil(char terminator) = 0;
    private:
    protected:
     bool connected;
};

#endif // STREAM_H
