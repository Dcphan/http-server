#include <cstdio>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <csignal>
#include "HttpServer.hpp"
#include "application/http_response.hpp"
#include "application/router.hpp"
#include "concurrency/threadPool.hpp"
#include "EventLoop.hpp"
#include <sys/eventfd.h>

const int MAX_CONNECTION = 128;
const int BUFFER_SIZE = 4096;

HttpServer::HttpServer(){
}


void HttpServer::run(int port){
    Socket server;

    if (server.setNonBlocking() < 0) {
        throw std::runtime_error("Failed to set server socket to non-blocking mode");
    }
    server.bind(port);
    server.listen(MAX_CONNECTION);

    // Created Share Queue
    TaskQueue<Task> requestQueue;
    ReadyQueue readyQueue;
    std::vector<std::unique_ptr<Socket>> fd2Socket;

    // Create eventfd
    int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (efd == -1) {
        throw std::runtime_error("eventfd creation failed");
    }
    ThreadPool threadPool;
    EventLoop eLoop;
    // Start workers
    threadPool.start(4, requestQueue, readyQueue, router, efd);

    // Run event loop on main thread
    eLoop.run(server, router, fd2Socket, requestQueue, readyQueue, efd);

 }


void HttpServer::GET(const std::string& path, Handler func){
    router.GET(path, func);
}

void HttpServer::POST(const std::string& path, Handler func){
    router.POST(path, func);
}

void HttpServer::PUT(const std::string& path, Handler func){
    router.PUT(path, func);
}

void HttpServer::DELETE(const std::string& path, Handler func){
    router.DELETE(path, func);
}
