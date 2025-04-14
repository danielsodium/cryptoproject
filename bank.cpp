#include "bank.h"
#include <iostream>

Bank::Bank() : server(Server(8080)) {
}

void Bank::Serve() {
    server.Start();
    std::string msg = server.Listen();
    std::cout << msg << std::endl;
    server.Send("Received: " + msg);
    // Initialize balance (for example, starting with $1000).
    int balance = 1000;
    while (true) {
        std::string cmd = server.Listen();
        if (cmd == "EXIT") {
            server.Send("Goodbye");
            break;
        } else if (cmd.find("DEPOSIT:") == 0) {
            int amount = std::stoi(cmd.substr(8));
            balance += amount;
            server.Send("Deposited " + std::to_string(amount) +
                        ". New balance: " + std::to_string(balance));
        } else if (cmd.find("WITHDRAW:") == 0) {
            int amount = std::stoi(cmd.substr(9));
            if (amount > balance) {
                server.Send("Insufficient funds.");
            } else {
                balance -= amount;
                server.Send("Withdrew " + std::to_string(amount) +
                            ". New balance: " + std::to_string(balance));
            }
        } else if (cmd == "BALANCE") {
            server.Send("Current balance: " + std::to_string(balance));
        } else {
            server.Send("Unknown command.");
        }
    }
    server.End();
}