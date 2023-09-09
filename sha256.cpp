#include "sha256.hpp"
#include <openssl/sha.h>

std::string sha256(const std::string &str) {
    std::string hash(SHA256_DIGEST_LENGTH, 0);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(reinterpret_cast<unsigned char *>(hash.data()), &sha256);

    return hash;
}