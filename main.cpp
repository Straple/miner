#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "assert.hpp"
#include "sha256.hpp"

std::string to_hex(uint64_t x, uint64_t len) {
    std::string ans;
    while (len) {
        len--;
        int digit = x % 16;
        x /= 16;
        if (digit < 10) {
            ans += '0' + digit;
        } else {
            ans += 'a' + digit - 10;
        }
    }
    std::reverse(ans.begin(), ans.end());
    return ans;
}

int hex_to_10(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else {
        return c - 'a' + 10;
    }
}

std::string to_bytes(uint64_t x, uint64_t length_bytes) {
    std::string result;
    while (length_bytes != 0) {
        length_bytes--;
        result += static_cast<char>(x % 256);
        x /= 256;
    }
    // std::reverse(result.begin(), result.end());
    return result;
}

std::string hex_to_bytes(std::string hexstr) {
    std::string result;
    uint64_t byte = 0;
    for (int i = 0; i + 1 < hexstr.size(); i += 2) {
        byte = hex_to_10(hexstr[i]) + 16 * hex_to_10(hexstr[i + 1]);
        ASSERT(byte < 256, "bad byte");
        result += static_cast<char>(byte);
    }
    if (hexstr.size() % 2 == 1) {
        result += static_cast<char>(hex_to_10(hexstr.back()));
    }
    return result;
}
std::string hex_to_bytes2(std::string hexstr) {
    std::string result;
    uint64_t byte = 0;
    for (int i = 0; i + 1 < hexstr.size(); i += 2) {
        byte = 16 * hex_to_10(hexstr[i]) + hex_to_10(hexstr[i + 1]);
        ASSERT(byte < 256, "bad byte");
        result += static_cast<char>(byte);
    }
    if (hexstr.size() % 2 == 1) {
        result += static_cast<char>(hex_to_10(hexstr.back()));
    }
    return result;
}

std::string hex_mult(std::string lhs, std::string rhs) {
    std::vector<uint64_t> data(lhs.size() + rhs.size() + 10, 0);
    for (int i = 0; i < lhs.size(); i++) {
        for (int j = 0; j < rhs.size(); j++) {
            data[i + j] += hex_to_10(lhs[i]) * hex_to_10(rhs[j]);
        }
    }

    for (int i = 0; i + 1 < data.size(); i++) {
        data[i + 1] += data[i] / 16;
        data[i] %= 16;
    }
    while (!data.empty() && data.back() == 0) {
        data.pop_back();
    }
    std::string ans;
    for (auto x : data) {
        ans += to_hex_symbol(x);
    }
    return ans;
}

std::string to_hex(std::string bytes) {
    std::string hex;
    for (unsigned char byte : bytes) {
        hex += to_hex_symbol(byte % 16);
        hex += to_hex_symbol(byte / 16);
    }
    return hex;
}

std::string reverse_object(const std::string &object) {
    std::string result = hex_to_bytes(object);
    std::reverse(result.begin(), result.end());
    result = to_hex(result);
    //std::reverse(result.begin(), result.end());
    return result;
}

struct block {
    uint32_t version;                 // 4 bytes
    std::string previous_block_hash;  // 32 bytes
    std::string merkle_root;          // 32 bytes
    uint32_t timestamp;               // 4 bytes
    uint32_t difficulty_target;       // 4 bytes
    uint32_t nonce;                   // 4 bytes

    // OK. worked
    std::string calc_target() const {
        // for more details and example see
        // https://en.bitcoin.it/wiki/Difficulty
        const uint64_t exponent = difficulty_target >> 24;
        const uint64_t mantissa = difficulty_target & 0xffffff;

        std::string x = "1";
        for (int i = 0; i < 8 * (exponent - 3); i++) {
            x = hex_mult(x, "2");
        }
        std::string target = hex_mult(to_hex(mantissa, 6), x);

        while (target.size() < 64) {
            target += '0';
        }
        std::reverse(target.begin(), target.end());

        // std::cout << target << '\n';
        return target;
    }

    std::string calc_hash() {
        std::string header =
            reverse_object(to_hex(version, 8)) +
            reverse_object(previous_block_hash) + reverse_object(merkle_root) +
            reverse_object(to_hex(timestamp, 8)) + reverse_object(to_hex(difficulty_target, 8)) +
            reverse_object(to_hex(nonce, 8));
        // header OK correct!!!

        //return header;
        // return reverse_object(sha256(hex_to_bytes2(header)));
        //return hex_to_bytes(header);
        //return reverse_object(to_hex(version, 8));
        auto s = sha256(hex_to_bytes2(header));
        for(int i = 0; i + 1 < s.size(); i += 2){
            //std::swap(s[i], s[i + 1]);
        }

        s = hex_to_bytes2(s);
        s = to_hex(s);
        return sha256(hex_to_bytes(s));

        //)));
        /*std::string version_buffer = to_bytes(version, 4);

        std::string previous_block_hash_buffer =
            hex_to_bytes(previous_block_hash);
        std::string merkle_root_buffer = hex_to_bytes(merkle_root);
        std::string timestamp_x_difficulty_target_x_nonce_buffer =
            to_bytes(timestamp, 4) + to_bytes(difficulty_target, 4) +
            to_bytes(nonce, 4);

        std::reverse(
            previous_block_hash_buffer.begin(), previous_block_hash_buffer.end()
        );
        std::reverse(merkle_root_buffer.begin(), merkle_root_buffer.end());

        std::string buffer = version_buffer + previous_block_hash_buffer +
                             merkle_root_buffer +
                             timestamp_x_difficulty_target_x_nonce_buffer;

        return sha256(sha256(buffer));*/
    }
};

int main() {
    block b = {
        2,
        "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717",
        "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a",
        0x53058b35,
        0x19015f53,
        856192328};

    std::cout << b.calc_hash() << '\n';
    auto s = b.calc_hash();
    // std::cout << s.size() << '\n';
    for (unsigned char c : s) {
         //std::cout << to_hex(c,2) << '\n';
    }

    /*while( nonce < 0x100000000){
    header = to_hex(version) + prev_block.decode('hex')[::-1] +
          mrkl_root.decode('hex')[::-1] + struct.pack("<LLL", time_, bits,
    nonce)) hash = hashlib.sha256(hashlib.sha256(header).digest()).digest()
    print nonce, hash[::-1].encode('hex')
    if hash[::-1] < target_str:
        print 'success'
        break
    nonce++;
    }*/
}