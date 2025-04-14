#include "hmac.h"
#include "sha.h"
#include <string>

std::string HMAC::generate(std::string key, std::string ctxt) {
    const int blockSize = 64;

    // If key is longer than block size, hash it
    if (key.length() > blockSize) {
        key = SHA::hash(key);
    }

    // Pad key with zeros to block size
    key.resize(blockSize, '\0');

    std::string o_key_pad(blockSize, '\0');
    std::string i_key_pad(blockSize, '\0');

    for (int i = 0; i < blockSize; ++i) {
        o_key_pad[i] = key[i] ^ 0x5c;
        i_key_pad[i] = key[i] ^ 0x36;
    }

    std::string inner_hash = SHA::hash(i_key_pad + ctxt);
    return SHA::hash(o_key_pad + inner_hash);
}
