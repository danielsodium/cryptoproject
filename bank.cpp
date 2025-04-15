#include "bank.h"
#include <iostream>
#include <unordered_map>
#include <sstream>

Bank::Bank() : server(Server(8080)) {
}

void Bank::Serve() {
    std::unordered_map<std::string, std::pair<std::string, double>> user_db = {
        {"alice", {"asdf", 1000.0}},
        {"bob", {"qwer", 750.0}}
    };

    server.Start();
    std::cout << "Established secure connection with ATM." << std::endl;

    bool authenticated = false;
    std::string current_user;

    while (true) {
        std::string msg = server.Listen();
        std::istringstream iss(msg);
        std::string command;
        iss >> command;

        if (command == "LOGIN") {
            std::string username, password;
            iss >> username >> password;

            if (user_db.find(username) != user_db.end() && user_db[username].first == password) {
                authenticated = true;
                current_user = username;
                server.Send("AUTH_SUCCESS");
            } else {
                server.Send("AUTH_FAILED");
            }
        } else if (!authenticated) {
            server.Send("NOT_AUTHENTICATED");
        } else if (command == "BALANCE") {
            double balance = user_db[current_user].second;
            server.Send(std::to_string(balance));
        } else if (command == "DEPOSIT") {
            double amount;
            iss >> amount;
            user_db[current_user].second += amount;
            server.Send("Deposit successful.");
        } else if (command == "WITHDRAW") {
            double amount;
            iss >> amount;
            if (user_db[current_user].second >= amount) {
                user_db[current_user].second -= amount;
                server.Send("Withdrawal successful.");
            } else {
                server.Send("Insufficient funds.");
            }
        } else if (command == "EXIT") {
            server.Send("Goodbye!");
            break;
        } else {
            server.Send("Unknown command.");
        }
    }

    server.End();
}
