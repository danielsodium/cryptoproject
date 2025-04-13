#ifndef DES_H 
#define DES_H 

#include <string> 

namespace DES {
    std::string encrypt(const std::string& ptxt, const std::string& key);
    std::string decrypt(const std::string& ctxt, const std::string& key);
}

#endif