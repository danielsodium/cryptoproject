#ifndef SERVER_H
#define SERVER_H

#include <string> 
#include <netinet/in.h>
#include "algos/ecc.h"

class Server {
public:
    Server(int p);
    void Start();
    std::string Listen();
    void Send(std::string msg);
    void End();

private:
    int port;
    int fd;
    sockaddr_in address;
    int connection;


    long private_key;
    ECC::Point public_key;
    std::string session_key;
};

#endif