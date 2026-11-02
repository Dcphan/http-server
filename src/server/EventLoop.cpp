#include "EventLoop.hpp"
#include "application/router.hpp"
#include "concurrency/queue.hpp"
#include "networking/socket.hpp"
#include "application/http_request.hpp"
#include <csignal>
#include <sys/eventfd.h>
#include <cerrno>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>
#include <memory>
#include <utility>
#include <cstddef>
#include <unistd.h>

constexpr int BUFFER_SIZE = 4096;
constexpr int MAX_ACCEPT_PER_LOOP = 64;

void EventLoop::run(Socket& server, Router&, std::vector<std::unique_ptr<Socket>>& fd2Socket, TaskQueue<Task>& taskQ, ReadyQueue& readyQ, int efd){
    this->pollArgs.push_back({server.getFd(), POLLIN, 0});

    this->pollArgs.push_back({efd, POLLIN, 0});
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
                std::unique_ptr<Socket> clientSocket = acceptConnections(server);
                if (clientSocket->getFd() < 0){
                    if (errno == EAGAIN || errno == EWOULDBLOCK) // 
                        break;
                    if (errno == EINTR)
                        continue;
                    break;
                }
                int clientFd = clientSocket->getFd();
                if (fd2Socket.size() <= static_cast<size_t>(clientFd)){
                    fd2Socket.resize(static_cast<size_t>(clientFd) + 1);
                }
                this->pollArgs.push_back({clientFd, POLLIN, 0});
                clientSocket->setPollIndex(this->pollArgs.size() - 1);
                fd2Socket[clientFd] = std::move(clientSocket);
            }
        }

        // Event Notification
        if(pollArgs[1].fd == efd && (pollArgs[1].revents & POLLIN)){
            uint64_t count;
            read(efd, &count, sizeof(count));

            ReadyTask task;

            while(readyQ.tryPop(task)){
                Socket& socket = *fd2Socket[task.fd];
                socket.appendWriteBuffer(task.responseString.c_str(), task.responseString.length());
                socket.setState(socketState::WRITING);
                pollArgs[socket.getPollIndex()].events |= POLLOUT;

            }
            
        }

        // Handling for Ready Client Socket
       for (size_t index = 2; index < pollArgs.size(); index++){
            struct pollfd& clientPfd = pollArgs[index];
            if (clientPfd.revents == 0)
                continue;

            if (clientPfd.fd < 0 || static_cast<size_t>(clientPfd.fd) >= fd2Socket.size() || !fd2Socket[clientPfd.fd]){
                continue;
            }

            Socket& clientSocket = *fd2Socket[clientPfd.fd];
            if (clientPfd.revents & POLLIN){
                handleRead(clientSocket);
                if (HttpRequest::isComplete(clientSocket.getReadBuffer())){
                        // Move to Task Queue
                    clientPfd.events &= ~POLLIN;
                    clientSocket.setState(socketState::PROCESSING);
                    Task task{clientSocket.getFd(), clientSocket.getReadBuffer()};
                    taskQ.push(std::move(task));                        
                }
            }

            if (clientPfd.revents & POLLOUT){
                WriteResult result = handleWrite(clientSocket);
                if (result == WriteResult::COMPLETE || result == WriteResult::ERROR){
                    clientPfd.events &= ~POLLIN;
                    clientSocket.setState(socketState::CLOSED);
                }
            }

            if (clientSocket.getState() == socketState::CLOSED){
               const int fd = clientSocket.getFd();
               removePollFd(static_cast<size_t>(clientSocket.getPollIndex()), fd2Socket);
               fd2Socket[fd].reset();
            }
       }
    }
}

std::unique_ptr<Socket> EventLoop::acceptConnections(Socket &server){
    std::unique_ptr<Socket> clientSocket = server.accept();

    if (clientSocket->getFd() < 0){
        // TODO: Adding to handling error
        return clientSocket;
    }

    clientSocket->setNonBlocking();
    return clientSocket;
}

void EventLoop::handleRead(Socket &clientSocket){
    char buffer[BUFFER_SIZE];
    int byteReceive = recv(clientSocket.getFd(), buffer, sizeof(buffer), 0);
    if (byteReceive > 0){
        clientSocket.appendReadBuffer(buffer, byteReceive);
        return;
    }

    if (byteReceive == 0){
        return;
    }

    if (errno == EAGAIN || errno == EWOULDBLOCK){
        return;
    }

    if (errno == EINTR){
        return;
    }
}

void EventLoop::removePollFd(size_t index, std::vector<std::unique_ptr<Socket>>& fd2Socket){
    if (index >= pollArgs.size()) {
        return;
    }

    size_t lastIndex = pollArgs.size() - 1;

    if (index != lastIndex) {
        pollArgs[index] = pollArgs[lastIndex];

        int movedFd = pollArgs[index].fd;
        fd2Socket[movedFd]->setPollIndex(index);
    }

    pollArgs.pop_back();    
}

WriteResult EventLoop::handleWrite(Socket &clientSocket){
    size_t currentOffset = clientSocket.getOffset();
    const std::string& writeBuffer = clientSocket.getWriteBuffer();
    while (currentOffset < writeBuffer.size()) {
        ssize_t bytesSent = send(
            clientSocket.getFd(),
            writeBuffer.data() + currentOffset,
            writeBuffer.size() - currentOffset,
            MSG_NOSIGNAL
        );

        if (bytesSent > 0) {
            currentOffset += static_cast<size_t>(bytesSent);
            clientSocket.incrementOffset(bytesSent);
        }
        else if (bytesSent == -1 && errno == EINTR) {
            continue;
        }
        else if (bytesSent == -1 &&
                 (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return WriteResult::WOULD_BLOCK;
        }
        else {
            return WriteResult::ERROR;
        }
}

    return WriteResult::COMPLETE; 
}
