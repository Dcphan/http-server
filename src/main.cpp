#include "networking/socket.hpp"
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <atomic>
#include <csignal>
#include "application/http_request.hpp"

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
       
        printf("Method: %s\n", request.method.c_str());
        printf("Path: %s\n", request.path.c_str());
        printf("Version: %s\n", request.version.c_str());
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
