#include <iostream>
#include <string>
#include <cstring>

#include "atm.h"
#include "bank.h"

#define PORT 8080

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Please provide an argument either ATM or BANK" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "ATM") == 0) {
        ATM a = ATM();
        a.Serve();
    }
    else if (strcmp(argv[1], "BANK") == 0) {
        Bank b = Bank();
        b.Serve();
    }
    else {
        std::cout << "Please provide an argument either ATM or BANK" << std::endl;
        return 1;
    }

    return 0;
}