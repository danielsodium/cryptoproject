#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "client.h"

Client::Client(int p): port(p) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation error");
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
}

void Client::Connect(std::string address) {
    if (inet_pton(AF_INET, address.c_str(), &server.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported\n";
        return;
    }

    // Connect to server
    if (connect(sock, (sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection Failed");
        return;
    }
}

std::string Client::Listen() {
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, 1024);
        if (valread > 0) break;
    }
    return std::string(buffer);
}

void Client::Send(std::string msg) {
    send(sock, msg.c_str(), msg.size(), 0);
}


void Client::Disconnect() {
    close(sock);
}