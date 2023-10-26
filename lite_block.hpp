#pragma once

#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "sha256/openssl_sha256.hpp"
#include <cstring>
#include <string>
#include <openssl/sha.h>

class lite_block {
    char bytes_data[80];

    SHA256_CTX save_sha256;

public:
    lite_block() = default;
    lite_block(const std::string &bytes_data_) {
        ASSERT(bytes_data_.size() == 80, "bad bytes data in init lite_block");
        std::memcpy(bytes_data, bytes_data_.c_str(), 80);

        SHA256_Init(&save_sha256);
        SHA256_Update(&save_sha256, bytes_data, 76);
    }

    std::string calc_hash(uint32_t x) {
        bytes_data[76] = x & 0xff;
        bytes_data[77] = (x >> 8) & 0xff;
        bytes_data[78] = (x >> 16) & 0xff;
        bytes_data[79] = (x >> 24) & 0xff;


        char hash[SHA256_DIGEST_LENGTH];

        SHA256_CTX cur_sha256 = save_sha256;
        SHA256_Update(&cur_sha256, bytes_data + 76, 4);
        SHA256_Final(reinterpret_cast<unsigned char *>(hash), &cur_sha256);

        SHA256_CTX new_sha256;
        SHA256_Init(&new_sha256);
        SHA256_Update(&new_sha256, hash, SHA256_DIGEST_LENGTH);
        SHA256_Final(reinterpret_cast<unsigned char *>(hash), &new_sha256);

        return reverse_str(std::string(hash, SHA256_DIGEST_LENGTH));
        //return reverse_str(sha256(sha256(bytes_data))); // было
    }

    // возвращает байтовое представление nonce
    uint32_t get_nonce() {
        uint32_t nonce = 0;
        nonce |= static_cast<uint32_t>(static_cast<uint8_t>(bytes_data[76]));
        nonce |= static_cast<uint32_t>(static_cast<uint8_t>(bytes_data[77]) << 8);
        nonce |= static_cast<uint32_t>(static_cast<uint8_t>(bytes_data[78]) << 16);
        nonce |= static_cast<uint32_t>(static_cast<uint8_t>(bytes_data[79]) << 24);
        return nonce;
    }
};
