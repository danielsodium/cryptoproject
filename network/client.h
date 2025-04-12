#ifndef CLIENT_H
#define CLIENT_H

#include <string> 
#include <netinet/in.h>

class Client {
public:
    Client(int p);
    void Connect(std::string address);
    void Send(std::string msg);
    std::string Listen();
    void Disconnect();

private:
    int port;
    int sock;
    sockaddr_in server;
};

#endif