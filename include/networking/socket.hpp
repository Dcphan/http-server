#pragma once

class Socket {
public:
    Socket();
    ~Socket();

    void bind(int port);
    void listen(int backlog);
    int accept();

private:
    int fd;
};
