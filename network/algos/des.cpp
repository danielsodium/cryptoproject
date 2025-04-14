#include "des.h"
#include <vector>
#include <bitset>
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

namespace DES {

// DES Key Schedule Tables
static const int PC1[56] = {
    57, 49, 41, 33, 25, 17, 9, 1,
    58, 50, 42, 34, 26, 18, 10, 2,
    59, 51, 43, 35, 27, 19, 11, 3,
    60, 52, 44, 36, 63, 55, 47, 39,
    31, 23, 15, 7, 62, 54, 46, 38,
    30, 22, 14, 6, 61, 53, 45, 37,
    29, 21, 13, 5, 28, 20, 12, 4
};

static const int PC2[48] = {
    14, 17, 11, 24, 1, 5, 3, 28,
    15, 6, 21, 10, 23, 19, 12, 4,
    26, 8, 16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56,
    34, 53, 46, 42, 50, 36, 29, 32
};

// DES Initial Permutation Table
static const int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

// DES Final Permutation Table (IP^-1)
static const int IP_INV[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

// DES Expansion Table
static const int E[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

// DES S-boxes
static const int S[8][4][16] = {
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
};

// DES P-box permutation
static const int P[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9,
    19, 13, 30, 6, 22, 11, 4, 25
};

static const int KEY_SHIFTS[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

const int BLOCK_SIZE = 8; 

std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
std::uniform_int_distribution<int> dist(0, 255);

std::string generateRandomIV() {
    std::string iv;
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        iv.push_back(static_cast<char>(dist(rng)));
    }
    return iv;
}

std::vector<bool> stringToBits(const std::string& str) {
    std::vector<bool> bits;
    for (char c : str) {
        for (int i = 7; i >= 0; --i) {
            bits.push_back((c >> i) & 1);
        }
    }
    return bits;
}

std::string bitsToString(const std::vector<bool>& bits) {
    std::string result;
    for (size_t i = 0; i < bits.size(); i += 8) {
        char byte = 0;
        for (int j = 0; j < 8 && i + j < bits.size(); ++j) {
            byte = (byte << 1) | bits[i + j];
        }
        result += byte;
    }
    return result;
}

std::vector<bool> permute(const std::vector<bool>& input, const int* table, int size) {
    std::vector<bool> output(size);
    for (int i = 0; i < size; ++i) {
        output[i] = input[table[i] - 1];
    }
    return output;
}

std::vector<bool> leftShift(const std::vector<bool>& bits, int shift) {
    std::vector<bool> result = bits;
    for (int i = 0; i < shift; ++i) {
        bool temp = result[0];
        for (size_t j = 0; j < result.size() - 1; ++j) {
            result[j] = result[j + 1];
        }
        result[result.size() - 1] = temp;
    }
    return result;
}

std::vector<bool> xorBits(const std::vector<bool>& a, const std::vector<bool>& b) {
    std::vector<bool> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] ^ b[i];
    }
    return result;
}

std::vector<bool> sBoxSubstitution(const std::vector<bool>& input) {
    std::vector<bool> output(32);
    
    for (int i = 0; i < 8; ++i) {
        int row = (input[i * 6] << 1) | input[i * 6 + 5];
        int col = (input[i * 6 + 1] << 3) | (input[i * 6 + 2] << 2) | 
                  (input[i * 6 + 3] << 1) | input[i * 6 + 4];
        
        int value = S[i][row][col];
        
        for (int j = 0; j < 4; ++j) {
            output[i * 4 + j] = (value >> (3 - j)) & 1;
        }
    }
    
    return output;
}

std::vector<std::vector<bool>> generateSubkeys(const std::vector<bool>& key) {
    std::vector<bool> permutedKey = permute(key, PC1, 56);
    
    std::vector<bool> left(permutedKey.begin(), permutedKey.begin() + 28);
    std::vector<bool> right(permutedKey.begin() + 28, permutedKey.end());
    
    std::vector<std::vector<bool>> subkeys(16);
    
    for (int i = 0; i < 16; ++i) {
        left = leftShift(left, KEY_SHIFTS[i]);
        right = leftShift(right, KEY_SHIFTS[i]);
        
        std::vector<bool> combined;
        combined.insert(combined.end(), left.begin(), left.end());
        combined.insert(combined.end(), right.begin(), right.end());
        
        subkeys[i] = permute(combined, PC2, 48);
    }
    
    return subkeys;
}

std::vector<bool> roundFunction(const std::vector<bool>& right, const std::vector<bool>& subkey) {
    std::vector<bool> expanded = permute(right, E, 48);
    
    std::vector<bool> xored = xorBits(expanded, subkey);
    
    std::vector<bool> substituted = sBoxSubstitution(xored);
    
    return permute(substituted, P, 32);
}

std::vector<bool> processBlock(const std::vector<bool>& block, const std::vector<std::vector<bool>>& subkeys, bool decrypt) {
    std::vector<bool> permuted = permute(block, IP, 64);
    
    std::vector<bool> left(permuted.begin(), permuted.begin() + 32);
    std::vector<bool> right(permuted.begin() + 32, permuted.end());
    
    for (int i = 0; i < 16; ++i) {
        int keyIndex = decrypt ? 15 - i : i;
        
        std::vector<bool> temp = right;
        std::vector<bool> fResult = roundFunction(right, subkeys[keyIndex]);
        right = xorBits(left, fResult);
        left = temp;
    }
    
    std::vector<bool> combined;
    combined.insert(combined.end(), right.begin(), right.end());
    combined.insert(combined.end(), left.begin(), left.end());
    
    return permute(combined, IP_INV, 64);
}

std::vector<bool> addPKCS7Padding(const std::vector<bool>& input) {
    int blockSizeBits = BLOCK_SIZE * 8;
    int paddingBytes = BLOCK_SIZE - (input.size() / 8) % BLOCK_SIZE;
    if (paddingBytes == 0) {
        paddingBytes = BLOCK_SIZE;
    }
    
    std::vector<bool> padded = input;
    // Add padding bytes
    for (int i = 0; i < paddingBytes; ++i) {
        char padValue = static_cast<char>(paddingBytes);
        for (int j = 7; j >= 0; --j) {
            padded.push_back((padValue >> j) & 1);
        }
    }
    
    return padded;
}

std::vector<bool> removePKCS7Padding(const std::vector<bool>& input) {
    if (input.empty()) {
        return input;
    }
    
    int size = input.size();
    std::vector<bool> lastByte(input.end() - 8, input.end());
    std::string byteStr = bitsToString(lastByte);
    int paddingBytes = static_cast<unsigned char>(byteStr[0]);
    
    if (paddingBytes > BLOCK_SIZE || paddingBytes == 0) {
        return input;
    }
    
    for (int i = 1; i <= paddingBytes; ++i) {
        std::vector<bool> padByte(input.end() - i * 8, input.end() - (i-1) * 8);
        std::string padByteStr = bitsToString(padByte);
        if (static_cast<unsigned char>(padByteStr[0]) != paddingBytes) {
            return input;
        }
    }
    
    return std::vector<bool>(input.begin(), input.end() - paddingBytes * 8);
}

std::string encrypt(const std::string& ptxt, const std::string& key) {
    std::string iv = generateRandomIV();
    
    // Convert input to bits
    std::vector<bool> plaintextBits = stringToBits(ptxt);
    std::vector<bool> keyBits = stringToBits(key);
    std::vector<bool> ivBits = stringToBits(iv);
    
    if (keyBits.size() < 64) {
        keyBits.resize(64, false);
    } else if (keyBits.size() > 64) {
        keyBits.resize(64);
    }
    
    std::vector<std::vector<bool>> subkeys = generateSubkeys(keyBits);
    
    std::vector<bool> paddedPlaintext = addPKCS7Padding(plaintextBits);
    
    std::vector<bool> ciphertextBits;
    std::vector<bool> previousBlock = ivBits;
    
    ciphertextBits.insert(ciphertextBits.end(), ivBits.begin(), ivBits.end());
    
    for (size_t i = 0; i < paddedPlaintext.size(); i += 64) {
        std::vector<bool> block(paddedPlaintext.begin() + i, 
                              i + 64 <= paddedPlaintext.size() 
                              ? paddedPlaintext.begin() + i + 64 
                              : paddedPlaintext.end());
        
        if (block.size() < 64) {
            block.resize(64, false);
        }
        
        std::vector<bool> xoredBlock = xorBits(block, previousBlock);
        
        std::vector<bool> encryptedBlock = processBlock(xoredBlock, subkeys, false);
        
        previousBlock = encryptedBlock;
        
        ciphertextBits.insert(ciphertextBits.end(), encryptedBlock.begin(), encryptedBlock.end());
    }
    
    return bitsToString(ciphertextBits);
}

std::string decrypt(const std::string& ctxt, const std::string& key) {
    // Convert input to bits
    std::vector<bool> ciphertextBits = stringToBits(ctxt);
    std::vector<bool> keyBits = stringToBits(key);
    
    if (keyBits.size() < 64) {
        keyBits.resize(64, false);
    } else if (keyBits.size() > 64) {
        keyBits.resize(64);
    }
    
    if (ciphertextBits.size() < 64) {
        return "";
    }
    
    std::vector<bool> ivBits(ciphertextBits.begin(), ciphertextBits.begin() + 64);
    std::vector<bool> actualCiphertext(ciphertextBits.begin() + 64, ciphertextBits.end());
    
    std::vector<std::vector<bool>> subkeys = generateSubkeys(keyBits);
    
    std::vector<bool> plaintextBits;
    std::vector<bool> previousBlock = ivBits; 
    
    for (size_t i = 0; i < actualCiphertext.size(); i += 64) {
        if (i + 64 > actualCiphertext.size()) {
            break;
        }
        
        std::vector<bool> block(actualCiphertext.begin() + i, actualCiphertext.begin() + i + 64);
        
        std::vector<bool> decryptedBlock = processBlock(block, subkeys, true);
        
        std::vector<bool> xoredBlock = xorBits(decryptedBlock, previousBlock);
        
        previousBlock = block;
        
        plaintextBits.insert(plaintextBits.end(), xoredBlock.begin(), xoredBlock.end());
    }
    
    std::vector<bool> unpaddedPlaintext = removePKCS7Padding(plaintextBits);
    
    return bitsToString(unpaddedPlaintext);
}
}