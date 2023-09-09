#pragma once

#include <string>

struct block {
    uint32_t version;                 // 4 bytes
    std::string previous_block_hash;  // 32 bytes
    std::string merkle_root;          // 32 bytes
    uint32_t timestamp;               // 4 bytes
    uint32_t difficulty_target;       // 4 bytes
    uint32_t nonce;                   // 4 bytes

    std::string save_bytes_data;

    void build_data();

    [[nodiscard]] std::string calc_target() const;

    std::string calc_hash(uint32_t nonce);
};