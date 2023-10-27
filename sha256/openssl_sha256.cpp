#include "openssl_sha256.hpp"
#include <openssl/sha.h>

fast_string sha256(const std::string &str) {
    fast_string hash(SHA256_DIGEST_LENGTH);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(reinterpret_cast<unsigned char *>(hash.c_str()), &sha256);

    return hash;
}

fast_string sha256(const fast_string &str) {
    fast_string hash(SHA256_DIGEST_LENGTH);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(reinterpret_cast<unsigned char *>(hash.c_str()), &sha256);

    return hash;
}