#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "algos/des.h"
#include "algos/rng.h"
#include "algos/ecc.h"
#include "algos/sha.h"
#include "algos/hmac.h"
#include "client.h"

Client::Client(int p): port(p), session_key("0") {
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

    // Secure the connection
    private_key = RNG::generate();
    public_key = ECC::multiply(private_key, ECC::generator);

    Send(std::to_string(public_key));

    long server_key;
    server_key = std::stol(Listen());

    long shared = ECC::multiply(private_key, server_key);

    session_key = SHA::hash(std::to_string(shared).substr(0,16));
}

std::string Client::Listen() {
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, 1024);
        if (valread > 0) break;
    }

    // Check HMAC auth
    std::string res = std::string(buffer);
    std::string tag = res.substr(0, 64);
    std::string ctxt = res.substr(64);
    if (HMAC::generate(session_key, ctxt) != tag) {
        std::cout << "Unauthenticated message" << std::endl;
        return "";
    }

    std::string ptxt = DES::decrypt(ctxt, session_key);
    std::cout << ptxt << std::endl;
    return ptxt;
}

void Client::Send(std::string msg) {
    std::string ctxt = DES::encrypt(msg, session_key);
    std::string tagged = HMAC::generate(session_key, ctxt) + ctxt;
    send(sock, tagged.c_str(), tagged.size(), 0);
}


void Client::Disconnect() {
    close(sock);
}