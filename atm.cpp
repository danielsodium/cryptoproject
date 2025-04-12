#include "atm.h"
#include <iostream>

ATM::ATM() : client(Client(8080)) {
}

void ATM::Serve() {
    client.Connect("127.0.0.1");

    std::string msg;
    std::getline(std::cin, msg);
    client.Send(msg);
    msg = client.Listen();
    std::cout << msg << std::endl;
    client.Disconnect();
}