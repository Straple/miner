#include "bits_manipulation.hpp"
#include "assert.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>

fast_string byte_to_hex(unsigned char byte) {
    std::stringstream ss;
    ss << std::hex << static_cast<int>(byte);
    return ss.str();
}

fast_string integer_to_hex(uint64_t integer, uint64_t len) {
    fast_string result;
    while (len != 0) {
        len--;
        int digit = integer % 16;
        integer /= 16;
        if (digit < 10) {
            result += '0' + digit;
        } else {
            result += 'a' + digit - 10;
        }
    }

    std::reverse(result.begin(), result.end());
    return result;
}

fast_string integer_to_bytes(uint64_t integer, uint64_t len) {
    fast_string result;
    while (len != 0) {
        len--;
        result += static_cast<char>(integer % 256);
        integer /= 256;
    }
    return result;
}

uint64_t hex_to_integer(char hex_code) {
    if ('0' <= hex_code && hex_code <= '9') {
        return hex_code - '0';
    } else {
        return hex_code - 'a' + 10;
    }
}

uint64_t hex_to_integer(fast_string hex_str) {
    uint64_t result = 0;
    for (char c: hex_str) {
        result *= 16;
        result += hex_to_integer(c);
    }
    return result;
}

fast_string hex_to_bytes(const fast_string &hex_str) {
    fast_string result;
    uint64_t byte = 0;
    for (int i = 0; i + 1 < hex_str.size(); i += 2) {
        byte = 16 * hex_to_integer(hex_str[i]) + hex_to_integer(hex_str[i + 1]);
        ASSERT(byte < 256, "bad byte");
        result += static_cast<char>(byte);
    }
    if (hex_str.size() % 2 == 1) {
        ASSERT(false, "why is not dividing by 2?");
        result += static_cast<char>(hex_to_integer(hex_str.back()));
    }
    return result;
}

std::string hex_to_bytes(const std::string &hex_str) {
    std::string result;
    uint64_t byte = 0;
    for (int i = 0; i + 1 < hex_str.size(); i += 2) {
        byte = 16 * hex_to_integer(hex_str[i]) + hex_to_integer(hex_str[i + 1]);
        ASSERT(byte < 256, "bad byte");
        result += static_cast<char>(byte);
    }
    if (hex_str.size() % 2 == 1) {
        ASSERT(false, "why is not dividing by 2?");
        result += static_cast<char>(hex_to_integer(hex_str.back()));
    }
    return result;
}

fast_string bytes_to_hex(const fast_string &bytes) {
    fast_string hex;
    //hex.reserve(bytes.size() * 2);
    for (unsigned char byte: bytes) {
        hex += byte_to_hex(byte >> 4);
        hex += byte_to_hex(byte & 0xf);
    }
    return hex;
}

std::string bytes_to_hex(const std::string &bytes) {
    std::string hex;
    //hex.reserve(bytes.size() * 2);
    for (unsigned char byte: bytes) {
        hex += byte_to_hex(byte >> 4).to_str();
        hex += byte_to_hex(byte & 0xf).to_str();
    }
    return hex;
}

fast_string hex_multiply(const fast_string &lhs, const fast_string &rhs) {
    std::vector<uint64_t> data(lhs.size() + rhs.size() + 10, 0);
    for (int i = 0; i < lhs.size(); i++) {
        for (int j = 0; j < rhs.size(); j++) {
            data[i + j] += hex_to_integer(lhs[i]) * hex_to_integer(rhs[j]);
        }
    }

    for (int i = 0; i + 1 < data.size(); i++) {
        data[i + 1] += data[i] / 16;
        data[i] %= 16;
    }
    while (!data.empty() && data.back() == 0) {
        data.pop_back();
    }
    fast_string ans;
    for (auto x: data) {
        ans += byte_to_hex(x);
    }
    return ans;
}

fast_string byte_reverse_in_hex(fast_string hex_str) {
    ASSERT(hex_str.size() % 2 == 0, "bad hex_str len");
    for (int i = 0; 2 * i < hex_str.size(); i += 2) {
        int l = 2 * i;
        int r = hex_str.size() - 2 * (i + 1);
        std::swap(hex_str[l], hex_str[r]);
        std::swap(hex_str[l + 1], hex_str[r + 1]);
    }
    return hex_str;
}

fast_string reverse_str(fast_string str) {
    std::reverse(str.begin(), str.end());
    return str;
}

uint32_t reverse_bytes(uint32_t x) {
    uint64_t res = 0;
    for (int i = 0; i < 4; i++) {
        uint32_t byte = (x >> (i * 8)) & 0xff;
        res |= byte << ((4 - i - 1) * 8);
    }
    return res;
}