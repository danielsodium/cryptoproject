#ifndef HMAC_H 
#define HMAC_H 

#include <string> 

namespace HMAC {
    std::string generate(std::string key, std::string ctxt);
}

#endif
