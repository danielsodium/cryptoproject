#ifndef SERVER_H
#define SERVER_H

#include <string> 
#include <netinet/in.h>

class Server {
public:
    Server(int p);
    std::string GetIP();
    void Start();
    std::string Listen();
    void Send(std::string msg);
    void End();

private:
    int port;
    int fd;
    sockaddr_in address;
    int connection;

    int private_key;
    int public_key;
    std::string session_key;
};

#endif