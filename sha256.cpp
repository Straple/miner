#include "sha256.hpp"
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

std::string to_hex_symbol(unsigned char s) {
    std::stringstream ss;
    ss << std::hex << static_cast<int>(s);
    return ss.str();
}

std::string sha256(const std::string &str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (unsigned char x : hash) {
        ss << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<int>(x);
    }
    return ss.str();
}