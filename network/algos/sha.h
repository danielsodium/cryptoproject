#ifndef SHA_H 
#define SHA_H 

#include <string> 

// SHA 256
namespace SHA {
    std::string hash(const std::string& input);
}

#endif