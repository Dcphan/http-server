#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <atomic>
#include <csignal>
#include "HttpServer.hpp"
#include "application/http_response.hpp"
#include "application/router.hpp"

const int MAX_CONNECTION = 5;
const int BUFFER_SIZE = 1024;

HttpServer::HttpServer(){
    this->router = new Router();
}

void handleClient(int clientSocket, Router& router)
{
    char buffer[BUFFER_SIZE];

    while (true)
    {

       int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            close(clientSocket);
            return;
        }

        // null terminator
        buffer[bytesReceived] = '\0';

        HttpRequest request = HttpRequest::parse(buffer);
        Handler httpHandler = router.find(request.method, request.path); 
        if (httpHandler == nullptr){
            // return error
        }
        HttpResponse response = httpHandler(request);
        std::string httpResp = HttpResponse::toHttpMessage(response);

        printf("Respond message: %s\n", httpResp.c_str());

        send(clientSocket, httpResp.data(), httpResp.size(), 0);
    }
    close(clientSocket);
}

void HttpServer::run(int port){
    Socket server;

    server.bind(8080);
    server.listen(MAX_CONNECTION);

    while (true) {
        int clientSocket = server.accept();
        
        printf("Client Accept\n");
        if (clientSocket < 0){
            continue;
        }

        // HANDLE CLIENT HERE
        char buffer[1024] = { 0 };

        handleClient(clientSocket, *router);  
    }
}


void HttpServer::GET(const std::string& path, Handler func){
    router->GET(path, func);
}

void HttpServer::POST(const std::string& path, Handler func){
    router->POST(path, func); 
}

void HttpServer::PUT(const std::string& path, Handler func){
    router->PUT(path, func);
}

void HttpServer::DELETE(const std::string& path, Handler func){
    router->DELETE(path, func);
}

