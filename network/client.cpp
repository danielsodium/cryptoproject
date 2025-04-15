#include <iostream>
#include <cstring>
#include <vector>
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

bool Client::Connect(std::string address) {
    if (inet_pton(AF_INET, address.c_str(), &server.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported\n";
        return false;
    }

    // Connect to server
    if (connect(sock, (sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection Failed");
        return false;
    }

    // Secure the connection
    private_key = RNG::generate() % 10000;
    public_key = ECC::multiply(ECC::generator, private_key);

    Send(ECC::serialize(public_key));

    ECC::Point server_key;
    std::string res = Listen();
    server_key = ECC::deserialize(res);

    ECC::Point shared = ECC::multiply(server_key, private_key);

    session_key = SHA::hash(std::to_string(shared.x).substr(0,16));
    std::cout << "SESSION KEY: " << session_key << std::endl;
    return true;
}

std::string Client::Listen() {
    uint32_t len;
    recv(sock, &len, sizeof(len), MSG_WAITALL);
    len = ntohl(len);
    
    std::vector<char> buffer(len);
    recv(sock, buffer.data(), len, MSG_WAITALL);
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

void Client::Send(std::string msg) {
    std::string ctxt = DES::encrypt(msg, session_key);
    std::string tag = HMAC::generate(session_key, ctxt);
    std::string tagged = tag + ctxt;
    uint32_t len = htonl(tagged.size());
    send(sock, &len, sizeof(len), 0);
    send(sock, tagged.c_str(), tagged.size(), 0);
}


void Client::Disconnect() {
    close(sock);
}