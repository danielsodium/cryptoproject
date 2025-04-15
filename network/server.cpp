#include <iostream>
#include <cstring>
#include <vector>
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

    // Secure the connection
    private_key = RNG::generate() % 10000;
    public_key = ECC::multiply(ECC::generator, private_key);

    ECC::Point client_key;
    client_key = ECC::deserialize(Listen());

    Send(ECC::serialize(public_key));

    ECC::Point shared = ECC::multiply(client_key, private_key);

    session_key = SHA::hash(std::to_string(shared.x).substr(0,16));
}

std::string Server::Listen() {
    uint32_t len;
    recv(connection, &len, sizeof(len), MSG_WAITALL); 
    len = ntohl(len); 
    
    std::vector<char> buffer(len);
    recv(connection, buffer.data(), len, MSG_WAITALL);
    std::string res(buffer.begin(), buffer.end());

    // Check HMAC auth
    std::string tag = res.substr(0, 64);
    std::string ctxt = res.substr(64);
    std::string expected = HMAC::generate(session_key, ctxt);
    if (expected != tag) {
        std::cout << "Unauthenticated message" << std::endl;
        return Listen();
    }

    std::string ptxt = DES::decrypt(ctxt, session_key);
    return ptxt;
}

void Server::Send(std::string msg) {
    std::string ctxt = DES::encrypt(msg, session_key);
    std::string tag = HMAC::generate(session_key, ctxt);
    std::string tagged = tag + ctxt;

    uint32_t len = htonl(tagged.size());
    send(connection, &len, sizeof(len), 0);
    send(connection, tagged.c_str(), tagged.size(), 0);
}

void Server::End() {
    close(connection);
    close(fd);
}