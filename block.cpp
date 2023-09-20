#include "block.hpp"
#include <algorithm>
#include "bits_manipulation.hpp"
#include "openssl_sha256.hpp"
#include "sha256_solver/sha256_solver.hpp"

[[nodiscard]] std::string block::calc_target() const {
    // for more details and example see
    // https://en.bitcoin.it/wiki/Difficulty
    const uint64_t exponent = nbits >> 24;
    const uint64_t mantissa = nbits & 0xffffff;

    std::string x = "1";
    for (int i = 0; i < 8 * (exponent - 3); i++) {
        x = hex_multiply(x, "2");
    }
    std::string target =
        hex_multiply(reverse_str(integer_to_hex(mantissa, 6)), x);

    while (target.size() < 64) {
        target += '0';
    }
    std::reverse(target.begin(), target.end());
    return hex_to_bytes(target);
}

std::pair<uint32_t, std::string> block::calc_hash(uint32_t x) {
    /*std::string extranonce2(extranonce2_size * 2, '0');  // random fill

    std::string coinbase = coinb1 + extranonce1 + extranonce2 + coinb2;

    // std::cout << "coinbase: " << coinbase << "\n\n";

    std::string coinbase_hash_bin = sha256(sha256(hex_to_bytes(coinbase)));

    merkle_root_hash = coinbase_hash_bin;
    for (auto hash : merkle_branch) {
        merkle_root_hash =
            sha256(sha256(merkle_root_hash + hex_to_bytes(hash)));
    }

    merkle_root_hash = byte_reverse_in_hex(bytes_to_hex(merkle_root_hash));
    // std::cout << "merkle_root: " << merkle_root_hash << "\n\n";

    std::string block_header =
        integer_to_hex(version, 8) + previous_block_hash + merkle_root_hash +
        integer_to_hex(timestamp, 8) + integer_to_hex(nbits, 8) +
        integer_to_hex(nonce, 8);

    ASSERT(block_header.size() == 2 * 80, "must be 80 bytes");

    return sha256(sha256(hex_to_bytes(block_header)));*/

    /*std::string header =
        byte_reverse_in_hex(integer_to_hex(version, 8)) +
        byte_reverse_in_hex(previous_block_hash) +
        byte_reverse_in_hex(merkle_root) +
        byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(difficulty_target, 8)) +
        byte_reverse_in_hex(integer_to_hex(nonce, 8));*/
    // return reverse_str(bytes_to_hex(sha256(sha256(hex_to_bytes(header)))));
    /*ASSERT(
        header ==
            "0200000017975b97c18ed1f7e255adf297599b55330edab87803c817010000"
            "00000000008a97295a2747b4f1a0b3948df3990344c0e19fa6b2b92b3a19c8"
            "e6badc141787358b0553535f011948750833",
        "bad header"
    );*/

    ASSERT((x >> 24) == 0, "bad x");

    for (int i = 0; i < 4; i++) {
        save_bytes_data[save_bytes_data.size() - 4 + i] = (x >> (8 * i)) & 0xff;
    }

    auto [best_x, hash] = find_best_hash(save_bytes_data);
    nonce = x | (best_x << 24);

    ASSERT(trivial_calc_hash(nonce) == hash, "failed");

    return {nonce, hash};
    //return reverse_str(sha256(sha256(save_bytes_data)));

    /*save_bytes_data = hex_to_bytes(
        byte_reverse_in_hex(integer_to_hex(version, 8)) +
        byte_reverse_in_hex(previous_block_hash) +
        byte_reverse_in_hex(merkle_root) +
        byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(difficulty_target, 8)) +
        std::string(8, '0')  // reserved by nonce
    );*/
}

std::string block::trivial_calc_hash(uint32_t nonce) {
    // std::string extranonce2(extranonce2_size * 2, '0');  // random fill

    std::string coinbase = coinb1 + extranonce1 + extranonce2 + coinb2;

    // std::cout << "coinbase: " << coinbase << "\n\n";

    std::string coinbase_hash_bin = sha256(sha256(hex_to_bytes(coinbase)));

    merkle_root_hash = coinbase_hash_bin;
    for (auto hash : merkle_branch) {
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

    std::string block_header =
        byte_reverse_in_hex(integer_to_hex(version, 8)) + previous_block_hash +
        merkle_root_hash + byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(nbits, 8)) +
        byte_reverse_in_hex(integer_to_hex(nonce, 8));

    ASSERT(block_header.size() == 2 * 80, "must be 80 bytes");

    // std::cout << "calc_triv: " << block_header << '\n';

    return reverse_str(sha256(sha256(hex_to_bytes(block_header))));
}

void block::build_data() {
    // std::string extranonce2(extranonce2_size * 2, '0');  // random fill

    std::string coinbase = coinb1 + extranonce1 + extranonce2 + coinb2;

    // std::cout << "coinbase: " << coinbase << "\n\n";

    std::string coinbase_hash_bin = sha256(sha256(hex_to_bytes(coinbase)));

    merkle_root_hash = coinbase_hash_bin;
    for (auto hash : merkle_branch) {
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

    std::string block_header =
        byte_reverse_in_hex(integer_to_hex(version, 8)) + previous_block_hash +
        merkle_root_hash + byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(nbits, 8)) +
        byte_reverse_in_hex(integer_to_hex(nonce, 8));

    ASSERT(block_header.size() == 2 * 80, "must be 80 bytes");

    // std::cout << "build_data:" << block_header << '\n';

    save_bytes_data = hex_to_bytes(block_header);
    /*save_bytes_data = hex_to_bytes(
        byte_reverse_in_hex(integer_to_hex(version, 8)) +
        byte_reverse_in_hex(previous_block_hash) +
        byte_reverse_in_hex(merkle_root) +
        byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(difficulty_target, 8)) +
        std::string(8, '0')  // reserved by nonce
    );*/
}