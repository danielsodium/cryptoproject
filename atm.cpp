#include "atm.h"
#include <iostream>

ATM::ATM() : client(Client(8080)) {}

void ATM::Serve() {
    std::string ip;
    std::string username;
    std::string password;
    std::string res;

    std::cout << "Enter ip address of bank (127.0.0.1): "; 
    std::getline(std::cin, ip);
    if (!client.Connect(ip)) {
        std::cerr << "Failed to connect to bank." << std::endl;
        return;
    }
    std::cout << "Established secure connection to bank" << std::endl;

    std::cout << "Enter username: ";
    std::getline(std::cin, username);
    std::cout << "Enter password: ";
    std::getline(std::cin, password);

    // Send login
    client.Send("LOGIN " + username + " " + password);
    res = client.Listen();

    if (res != "AUTH_SUCCESS") {
        std::cout << "Authentication failed." << std::endl;
        client.Disconnect();
        return;
    }

    std::cout << "Login successful!" << std::endl;

    while (true) {
        std::string choice;
        std::cout << "\n1. Check Balance\n2. Deposit\n3. Withdraw\n4. Exit\nEnter option: ";
        std::getline(std::cin, choice);

        if (choice == "1") {
            client.Send("BALANCE");
            res = client.Listen();
            std::cout << "Your balance is: $" << res << std::endl;
        } else if (choice == "2") {
            std::string amount;
            std::cout << "Enter amount to deposit: ";
            std::getline(std::cin, amount);
            client.Send("DEPOSIT " + amount);
            res = client.Listen();
            std::cout << res << std::endl;
        } else if (choice == "3") {
            std::string amount;
            std::cout << "Enter amount to withdraw: ";
            std::getline(std::cin, amount);
            client.Send("WITHDRAW " + amount);
            res = client.Listen();
            std::cout << res << std::endl;
        } else if (choice == "4") {
            client.Send("EXIT");
            break;
        } else {
            std::cout << "Invalid option." << std::endl;
        }
    }

    client.Disconnect();
}
