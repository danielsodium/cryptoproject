#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "algos/des.h"
#include "client.h"

Client::Client(int p): port(p), session_key("asdf") {
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

    // TODO: Secure Connection
}

std::string Client::Listen() {
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, 1024);
        if (valread > 0) break;
    }
    std::string ptxt = DES::decrypt(std::string(buffer), session_key);
    return ptxt;
}

void Client::Send(std::string msg) {
    std::string ctxt = DES::encrypt(msg, session_key);
    std::cout << "Sending " << ctxt << std::endl;
    send(sock, ctxt.c_str(), ctxt.size(), 0);
}


void Client::Disconnect() {
    close(sock);
}