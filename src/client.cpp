#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

static void die(const char *message){
    fprintf(stderr, "ERROR: %s\n", message);
    abort();
}

int main()
{
    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0){
        die("Socket");
    }

    // specifying address
    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int rv = connect(clientSocket, (const struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (rv < 0){
        die("Connect");
    }

    const char* message = "Hello, Server";
    send(clientSocket, message, strlen(message), 0);

    close(clientSocket);

    return 0;
}
