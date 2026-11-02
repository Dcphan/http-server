#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "networking/socket.hpp"

void die(const char *message){
   fprintf(stderr, "Error: %s\n", message);  
}

Socket::Socket(){
    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0){
        die("Socket");
    }
}

Socket::Socket(int fd) {
    this->fd = fd;
}

Socket::~Socket(){
    if (fd >= 0){
        close(fd);
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

int Socket::getFd(){
    return this->fd;
}

std::unique_ptr<Socket> Socket::accept(){
     struct sockaddr_in client_addr = {};
     socklen_t addr_len = sizeof(client_addr);
     return std::make_unique<Socket>(::accept(fd, (struct sockaddr*) &client_addr, &addr_len));
}

int Socket::setNonBlocking(){
    int flag = fcntl(this->fd, F_GETFL, 0);

    if (flag == -1){
        perror("Error Getting Flag");
        return -1;
    }

    if (fcntl(this->fd, F_SETFL, flag | O_NONBLOCK) < 0){
        perror("Error Setting Flag as NonBlock");
        return -1;
    }
    return 0;
}

socketState Socket::getState(){
    return currentState;
}

void Socket::setState(socketState state) {
    currentState = state;
}

void Socket::appendReadBuffer(const char* data, size_t length) {
    readBuffer.append(data, length);
}

const std::string& Socket::getReadBuffer() const {
    return readBuffer;
}

void Socket::clearReadBuffer() {
    readBuffer.clear();
}

void Socket::appendWriteBuffer(const char* data, size_t length) {
    writeBuffer.append(data, length);
}

const std::string& Socket::getWriteBuffer() const {
    return writeBuffer;
}

void Socket::clearWriteBuffer() {
    readBuffer.clear();
}

void Socket::setPollIndex(int index){
    pollIndex = index;
}

int Socket::getPollIndex(){
    return pollIndex;
}

void Socket::incrementOffset(size_t n){
    writeOffset += n;
}

size_t Socket::getOffset(){
    return writeOffset;
}
