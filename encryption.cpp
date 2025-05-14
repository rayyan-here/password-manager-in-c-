#include "encryption.h"

std::string xorEncryptDecrypt(const std::string& input) {
    std::string output = input;
    char key = 'K'; // simple XOR key
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] ^= key;
    }
    return output;
}
