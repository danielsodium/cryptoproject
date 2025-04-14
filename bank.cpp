#include "bank.h"
#include <iostream>

Bank::Bank() : server(Server(8080)) {
}

void Bank::Serve() {
    server.Start();
    std::string msg = server.Listen();
    std::cout << msg << std::endl;
    server.Send("Received: " + msg);
    server.End();
}