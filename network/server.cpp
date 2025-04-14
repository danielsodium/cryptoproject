#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

#include "algos/des.h"
#include "algos/rng.h"
#include "algos/ecc.h"
#include "algos/sha.h"
#include "algos/hmac.h"
#include "server.h"

Server::Server(int p) : port(p), session_key("0") {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind socket
    if (bind(fd, (sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void Server::Start() {
    int addrlen = sizeof(address);
    listen(fd, 3);
    std::cout << "Server listening on port " << port << "...\n";

    // Accept connection
    connection = accept(fd, (sockaddr *)&address, (socklen_t*)&addrlen);
    if (connection < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    private_key = RNG::generate();
    public_key = ECC::multiply(private_key, ECC::generator);

    long client_key;
    client_key = std::stol(Listen());

    Send(std::to_string(public_key));

    long shared = ECC::multiply(private_key, client_key);

    session_key = SHA::hash(std::to_string(shared).substr(0,16));
}

std::string Server::Listen() {
    // Holds until next message 
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(connection, buffer, 1024);
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
    return ptxt;
}

void Server::Send(std::string msg) {
    std::string ctxt = DES::encrypt(msg, session_key);
    std::string tagged = HMAC::generate(session_key, ctxt) + ctxt;
    send(connection, tagged.c_str(), tagged.size(), 0);
}

void Server::End() {
    close(connection);
    close(fd);
}