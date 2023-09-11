#include "block.hpp"
#include <algorithm>
#include "bits_manipulation.hpp"
#include "sha256.hpp"

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

std::string block::calc_hash(uint32_t nonce) {
    // random fill
    std::string extranonce2(extranonce2_size * 2, '0');

    std::string coinbase = coinb1 + extranonce1 + extranonce2 + coinb2;

    std::cout << "coinbase: " << coinbase << "\n\n";

    std::string coinbase_hash_bin = sha256(sha256(hex_to_bytes(coinbase)));

    merkle_root_hash = coinbase_hash_bin;
    for (auto hash : merkle_branch) {
        merkle_root_hash =
            sha256(sha256(merkle_root_hash + hex_to_bytes(hash)));
    }

    merkle_root_hash = byte_reverse_in_hex(bytes_to_hex(merkle_root_hash));
    std::cout << "merkle_root: " << merkle_root_hash << "\n\n";

    std::string block_header =
        integer_to_hex(version, 8) + previous_block_hash + merkle_root_hash +
        integer_to_hex(timestamp, 8) + integer_to_hex(nbits, 8) +
        integer_to_hex(nonce, 8);

    ASSERT(block_header.size() == 2 * 80, "must be 80 bytes");

    return bytes_to_hex(sha256(sha256(hex_to_bytes(block_header))));

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

    /*nonce = reverse_bytes(nonce);
    for (int i = 0; i < 4; i++) {
        save_bytes_data[save_bytes_data.size() - 4 + i] =
            (nonce >> (8 * (4 - i - 1))) & 0xff;
    }

    return reverse_str(sha256(sha256(save_bytes_data)));*/

    // return "";
    /*save_bytes_data = hex_to_bytes(
        byte_reverse_in_hex(integer_to_hex(version, 8)) +
        byte_reverse_in_hex(previous_block_hash) +
        byte_reverse_in_hex(merkle_root) +
        byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(difficulty_target, 8)) +
        std::string(8, '0')  // reserved by nonce
    );*/
}

void block::build_data() {
    /*save_bytes_data = hex_to_bytes(
        byte_reverse_in_hex(integer_to_hex(version, 8)) +
        byte_reverse_in_hex(previous_block_hash) +
        byte_reverse_in_hex(merkle_root) +
        byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(difficulty_target, 8)) +
        std::string(8, '0')  // reserved by nonce
    );*/
}