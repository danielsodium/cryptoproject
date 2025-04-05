#ifndef SERVER_H
#define SERVER_H

#include <string> 
#include <netinet/in.h>

class Server {
public:
    Server(int p);
    std::string GetIP();
    void Listen();

private:
    int port;
    int fd;
    sockaddr_in address;
    int connection;
};

#endif