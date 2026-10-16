#include "EventLoop.hpp"
#include "application/router.hpp"
#include "networking/socket.hpp"
#include "application/http_request.hpp"
#include <cerrno>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>

constexpr int BUFFER_SIZE = 4096;
constexpr int MAX_ACCEPT_PER_LOOP = 64;

void EventLoop::run(Socket* server, Router& router, std::vector<Socket *> fd2Socket){
    this->pollArgs.push_back({server->getFd(), POLLIN, 0});

    while (true){
        
        int ready = poll(pollArgs.data(), pollArgs.size(), -1);
        
        if (ready < 0){
            if (errno == EINTR)
                continue;
            break;
        }

        // SERVER SOCKET - Accepting New Connection 
        if (pollArgs[0].revents & POLLIN){
            // For Loop is implement for limiting the # of request
            // Preventing a server from becoming deadlock
            for (int count = 0; count < MAX_ACCEPT_PER_LOOP; count++){ 
                Socket* clientSocket = acceptConnections(server);
                if (clientSocket->getFd() < 0){
                    if (errno == EAGAIN || errno == EWOULDBLOCK) // 
                        break;
                    if (errno == EINTR)
                        continue;
                    break;
                }
                this->pollArgs.push_back({clientSocket->getFd(), POLLIN, 0});
                fd2Socket[clientSocket->getFd()] = clientSocket;
            }
        }

        // Handling for Ready Client Socket
       for (int index = 1; index < pollArgs.size(); index++){
            struct pollfd clientPfd = pollArgs[index];
            if (clientPfd.revents == 0)
                continue;

            if (clientPfd.revents & POLLIN){
                Socket* clientSocket = fd2Socket[clientPfd.fd];
                handleRead(clientSocket);
                if (HttpRequest::isComplete(clientSocket->getReadBuffer()){
                        // Move to Task Queue
                        
                    }
            }

            if (clientPfd.revents & POLLOUT){
                handleWrite(fd2Socket[clientPfd.fd]);
            }
       }
    }
}

Socket* acceptConnections(Socket* server){
    Socket* clientSocket = server->accept();  

    if (clientSocket->getFd() < 0){
        // TODO: Adding to handling error
        return clientSocket;
    }

    clientSocket->setNonBlocking();
    return clientSocket;
}

void handleRead(Socket* clientSocket){
    char buffer[BUFFER_SIZE];
    int byteReceive = recv(clientSocket->getFd(), buffer, sizeof(buffer), 0);
    clientSocket->appendReadBuffer(buffer, byteReceive);
}
