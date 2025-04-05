#include <iostream>
#include <string>
#include <cstring>

#include "client.h"
#include "server.h"

#define PORT 8080

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Please provide an argument either ATM or BANK" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "ATM") == 0) {
        Client a = Client(PORT);
        a.Connect("127.0.0.1");
        a.Send();
        a.Disconnect();
    }
    else if (strcmp(argv[1], "BANK") == 0) {
        Server a = Server(PORT);
        a.Listen();
    }
    else {
        std::cout << "Please provide an argument either ATM or BANK" << std::endl;
        return 1;
    }

    return 0;
}