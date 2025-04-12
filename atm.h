#ifndef ATM_H 
#define ATM_H 

#include <string> 
#include <netinet/in.h>
#include "network/client.h"

class ATM {
public:
    ATM();
    void Serve();

private:
    Client client;
};

#endif