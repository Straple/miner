#pragma once

#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "sha256/openssl_sha256.hpp"
#include <cstring>
#include <openssl/sha.h>
#include <string>

class lite_block {
    SHA256_CTX sha256_ctx;

public:
    lite_block() = default;
    lite_block(const fast_string &bytes_data);

    [[nodiscard]] fast_string calc_hash(uint32_t x) const;
};
