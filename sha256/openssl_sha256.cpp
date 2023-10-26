#include <openssl/sha.h>
#include "openssl_sha256.hpp"

std::string sha256(const std::string &str) {
    std::string hash(SHA256_DIGEST_LENGTH, 0);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(reinterpret_cast<unsigned char *>(hash.data()), &sha256);

    return hash;
}

std::string sha256(const char str[80]) {
    std::string hash(SHA256_DIGEST_LENGTH, 0);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, 80);
    SHA256_Final(reinterpret_cast<unsigned char *>(hash.data()), &sha256);

    return hash;
}