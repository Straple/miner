#include "block.hpp"
#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "sha256/openssl_sha256.hpp"
#include "utils.hpp"
#include <algorithm>

[[nodiscard]] fast_string block::calc_target() const {
    // for more details and example see
    // https://en.bitcoin.it/wiki/Difficulty
    const uint64_t exponent = nbits >> 24;
    const uint64_t mantissa = nbits & 0xffffff;

    fast_string x = std::string("1");
    for (int i = 0; i < 8 * (exponent - 3); i++) {
        x = hex_multiply(x, std::string("2"));
    }
    fast_string target =
            hex_multiply(reverse_str(integer_to_hex(mantissa, 6)), x);

    while (target.size() < 64) {
        target += '0';
    }
    std::reverse(target.begin(), target.end());
    return hex_to_bytes(target);
}

fast_string block::calc_hash(uint32_t x) {
    nonce = x;
    for (int i = 0; i < 4; i++) {
        save_bytes_data[80 - 4 + i] = (x >> (8 * i)) & 0xff;
    }
    return sha256(sha256(save_bytes_data));
}

// 51KH/s
fast_string block::trivial_calc_hash(uint32_t nonce) {
    // std::string extranonce2(extranonce2_size * 2, '0');  // random fill

    std::string coinbase = hex_to_bytes(coinb1) + hex_to_bytes(extranonce1) + hex_to_bytes(extranonce2.to_str()) + hex_to_bytes(coinb2);

    // std::cout << "coinbase: " << coinbase << "\n\n";

    fast_string coinbase_hash_bin = sha256(sha256(coinbase));

    merkle_root_hash = coinbase_hash_bin;
    for (auto hash: merkle_branch) {
        merkle_root_hash =
                sha256(sha256(merkle_root_hash + hex_to_bytes(hash)));
    }

    merkle_root_hash = byte_reverse_in_hex(bytes_to_hex(merkle_root_hash));
    // std::cout << "merkle_root: " << merkle_root_hash << "\n\n";

    // ===============
    /*version = 2;
    previous_block_hash =
    "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717";
    merkle_root_hash =
    "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a";
    timestamp = 1392872245;
    nbits = 0x19015f53;*/
    // ===============

    fast_string block_header =
            hex_to_bytes(byte_reverse_in_hex(integer_to_hex(version, 8))) +
            hex_to_bytes(previous_block_hash) +
            hex_to_bytes(merkle_root_hash) +
            hex_to_bytes(byte_reverse_in_hex(integer_to_hex(timestamp, 8))) +
            hex_to_bytes(byte_reverse_in_hex(integer_to_hex(nbits, 8))) +
            hex_to_bytes(byte_reverse_in_hex(integer_to_hex(nonce, 8)));

    ASSERT(block_header.size() == 80, "must be 80 bytes");

    // std::cout << "calc_triv: " << block_header << '\n';

    return sha256(sha256(block_header));
}

void block::build_data() {
    // std::string extranonce2(extranonce2_size * 2, '0');  // random fill

    std::string coinbase = hex_to_bytes(coinb1) + hex_to_bytes(extranonce1) + hex_to_bytes(extranonce2.to_str()) + hex_to_bytes(coinb2);

    // std::cout << "coinbase: " << coinbase << "\n\n";

    fast_string coinbase_hash_bin = sha256(sha256(coinbase));

    merkle_root_hash = coinbase_hash_bin;
    for (auto hash: merkle_branch) {
        merkle_root_hash = sha256(sha256(merkle_root_hash + hex_to_bytes(hash)));
    }

    merkle_root_hash = byte_reverse_in_hex(bytes_to_hex(merkle_root_hash));
    // std::cout << "merkle_root: " << merkle_root_hash << "\n\n";

    // ===============
    /*version = 2;
    previous_block_hash =
    "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717";
    merkle_root_hash =
    "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a";
    timestamp = 1392872245;
    nbits = 0x19015f53;*/
    // ===============

    fast_string block_header =
            hex_to_bytes(byte_reverse_in_hex(integer_to_hex(version, 8))) +
            hex_to_bytes(previous_block_hash) +
            hex_to_bytes(merkle_root_hash) +
            hex_to_bytes(byte_reverse_in_hex(integer_to_hex(timestamp, 8))) +
            hex_to_bytes(byte_reverse_in_hex(integer_to_hex(nbits, 8))) +
            hex_to_bytes(byte_reverse_in_hex(integer_to_hex(nonce, 8)));

    ASSERT(block_header.size() == 80, "must be 80 bytes");

    // std::cout << "build_data:" << block_header << '\n';

    save_bytes_data = block_header;
    /*save_bytes_data = hex_to_bytes(
        byte_reverse_in_hex(integer_to_hex(version, 8)) +
        byte_reverse_in_hex(previous_block_hash) +
        byte_reverse_in_hex(merkle_root) +
        byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(difficulty_target, 8)) +
        std::string(8, '0')  // reserved by nonce
    );*/
}

void block::build_extranonce2() {
    extranonce2 = integer_to_hex(rnd(), extranonce2_size * 2);
    build_data();
}
