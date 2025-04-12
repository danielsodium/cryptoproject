#ifndef BANK_H 
#define BANK_H

#include <string> 
#include <netinet/in.h>
#include "network/server.h"

class Bank {
public:
    Bank();
    void Serve();

private:
    Server server;
};

#endif