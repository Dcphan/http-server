#pragma once
#include "string"
#include <string>

enum class socketState {
        READING,
        PROCESSING,
        WRITING,
        CLOSED
    };

class Socket {
public:
    Socket();
    ~Socket();

    void bind(int port);
    void listen(int backlog);
    int setNonBlocking();
    int getFd();
    Socket* accept();

    socketState getState();
    void setState(socketState state);

    void appendReadBuffer(const char* data, size_t length);
    const std::string& getReadBuffer() const;
    void clearReadBuffer();

private:
    int fd;
    std::string readBuffer = "";
    std::string writeBuffer = "";
    socketState currentState = socketState::READING;
};
