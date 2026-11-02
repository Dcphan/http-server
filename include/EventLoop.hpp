#pragma once

#include "application/router.hpp"
#include "networking/socket.hpp"
#include <vector>
#include <poll.h>

class EventLoop {
public:
    void run(Socket* server, Router& router, std::vector<Socket *>fd2Socket);

private:
    Socket* acceptConnections(Socket* serverFd);
    void handleRead(Socket* clientSocket);
    void handleWrite(Socket* clientSocket);
    void closeConnection(Socket* clientSocket);
    std::vector<pollfd> pollArgs;
};
