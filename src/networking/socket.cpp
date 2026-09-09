#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "unistd.h"
#include "socket.hpp"

void die(const char *message){
   fprintf(stderr, "Error: %s\n", message);  
}

Socket::Socket(){
    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0){
        die("Socket");
    }
}

void Socket::bind(int port){
    struct sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int rv = ::bind(fd, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
    if (rv < 0){
       die("Bind"); 
    }
}

void Socket::listen(int backlog){
    int rv = ::listen(fd, backlog);

    if (rv < 0){
        die("Listen");
    }
}

int Socket::accept(){
     struct sockaddr_in client_addr = {};
     socklen_t addr_len = sizeof(client_addr);
     int clientSocket = ::accept(fd, (struct sockaddr*) &client_addr, &addr_len);
     return clientSocket;
}

 Socket::~Socket(){
    close(fd);
}
