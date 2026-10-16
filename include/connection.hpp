#pragma once
#include <string>
#include "networking/socket.hpp"

struct Connection {
    Socket socketFd;

    std::string incomingBuffer;
    std::string outcomingBuffer;

    enum class State {
        READING,
        PROCESSING,
        WRITING,
        CLOSED,
    };
    State state = State::READING;
};
