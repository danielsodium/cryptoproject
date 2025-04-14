#include "atm.h"
#include <iostream>

ATM::ATM() : client(Client(8080)) {
}

void ATM::Serve() {
    // Prompt for IP address and address
    std::string ip;
    std::string username;
    std::string password;
    std::string res;

    std::cout << "Enter ip address of bank: " << std::endl;
    std::getline(std::cin, ip);
    client.Connect(ip);
    std::cout << "Established secure connection to bank" << std::endl;

    // Prompt for username
    std::cout << "Enter username: " << std::endl;
    std::getline(std::cin, username);
    
    client.Send(username);
    res = client.Listen();
   

    // Send initial message
    std::string msg;
    std::getline(std::cin, msg);
    client.Send(msg);
    msg = client.Listen();
    std::cout << msg << std::endl;
    client.Disconnect();
}