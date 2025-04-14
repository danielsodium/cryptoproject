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
   
    std::string response = client.Listen();
    std::cout << response << std::endl;
    
    // Command loop for banking operations.
    while (true) {
        std::cout << "Enter command (DEPOSIT:<amount>, WITHDRAW:<amount>, BALANCE, EXIT): ";
        std::string cmd;
        std::getline(std::cin, cmd);
        client.Send(cmd);
        response = client.Listen();
        std::cout << response << std::endl;
        if (cmd == "EXIT")
            break;
    }
    client.Disconnect();
}