#include "networking/socket.hpp"
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <atomic>
#include <csignal>
#include "application/request_handler.hpp"

const int MAX_CONNECTION = 5;
const int BUFFER_SIZE = 1024;


void handleClient(int clientSocket)
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
        HttpResponse response = RequestHandler::handler(request);
        std::string httpResp = HttpResponse::toHttpMessage(response);

        printf("Respond message: %s\n", httpResp.c_str());

        send(clientSocket, httpResp.data(), httpResp.size(), 0);
    }
    close(clientSocket);
}

int main(){
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

        handleClient(clientSocket);  
    }

    return 0;
}
