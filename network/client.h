#ifndef CLIENT_H
#define CLIENT_H

#include <string> 
#include <netinet/in.h>
#include "algos/ecc.h"

class Client {
public:
    Client(int p);
    bool Connect(std::string address);
    void Send(std::string msg);
    std::string Listen();
    void Disconnect();

private:
    int port;
    int sock;
    sockaddr_in server;

    long private_key;
    ECC::Point public_key;
    std::string session_key;
};

#endif