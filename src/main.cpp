#include "networking/socket.hpp"
#include <cstdio>
#include <sys/socket.h>

const int MAX_CONNECTION = 5;


int main(){
    Socket server;

    server.bind(8080);
    server.listen(MAX_CONNECTION);

    while (true) {
        int clientSocket = server.accept();
        
        if (clientSocket < 0){
            continue;
        }

        // HANDLE CLIENT HERE
        char buffer[1024] = { 0 };
        recv(clientSocket, buffer, sizeof(buffer), 0);

        fprintf(stdout, "Message from client: %s\n", buffer);
    }

    return 0;
}
