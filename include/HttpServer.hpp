#pragma once
#include "networking/socket.hpp"
#include <sys/socket.h>
#include <vector>
#include "application/router.hpp"

class HttpServer{
public:
    HttpServer();
    void run(int port);
    void GET(const std::string& path, Handler func);
    void POST(const std::string& path, Handler func);
    void PUT(const std::string& path, Handler func);
    void DELETE(const std::string& path, Handler func);

private:
    Router* router;
    std::vector<Socket *> fd2Socket;
};
