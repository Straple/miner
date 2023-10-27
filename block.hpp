#pragma once

#include <string>
#include <vector>

#include "lite_block.hpp"
#include "fast_string.hpp"

struct block {
    uint32_t version=0;               // 4 bytes
    fast_string previous_block_hash;// 32 bytes
    fast_string merkle_root_hash;   // 32 bytes
    uint32_t timestamp=0;             // 4 bytes
    uint32_t nbits=0;                 // 4 bytes
    uint32_t nonce=0;                 // 4 bytes

    fast_string extranonce2;

    std::vector<fast_string> merkle_branch;
    std::string coinb1, coinb2, extranonce1;
    int extranonce2_size=0;

    fast_string save_bytes_data;

    void build_data();

    [[nodiscard]] fast_string calc_target() const;

    fast_string trivial_calc_hash(uint32_t nonce);

    fast_string calc_hash(uint32_t x);

    void build_extranonce2();

    lite_block build_lite_block() {
        return lite_block(save_bytes_data);
    }
};