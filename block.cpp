#include "block.hpp"
#include <algorithm>
#include "bits_manipulation.hpp"
#include "sha256.hpp"

[[nodiscard]] std::string block::calc_target() const {
    // for more details and example see
    // https://en.bitcoin.it/wiki/Difficulty
    const uint64_t exponent = difficulty_target >> 24;
    const uint64_t mantissa = difficulty_target & 0xffffff;

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
    return target;
}

std::string block::calc_hash(uint32_t nonce) {
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

    nonce = reverse_bytes(nonce);
    for (int i = 0; i < 4; i++) {
        save_bytes_data[save_bytes_data.size() - 4 + i] =
            (nonce >> (8 * (4 - i - 1))) & 0xff;
    }

    return reverse_str(bytes_to_hex(sha256(sha256(save_bytes_data))));
}

void block::build_data() {
    save_bytes_data = hex_to_bytes(
        byte_reverse_in_hex(integer_to_hex(version, 8)) +
        byte_reverse_in_hex(previous_block_hash) +
        byte_reverse_in_hex(merkle_root) +
        byte_reverse_in_hex(integer_to_hex(timestamp, 8)) +
        byte_reverse_in_hex(integer_to_hex(difficulty_target, 8)) +
        std::string(8, '0')  // reserved by nonce
    );
}