#include "sha256_solver.hpp"
#include "sha256.hpp"
#include <algorithm>

std::vector<byte_t> calc_sha256(const std::vector<byte_t> &data) {
    SHA256 sha;
    sha.update(data);
    auto digest = sha.digest();
    return digest;
}

std::vector<byte_t> build_digest(const std::string &bytes) {
    std::vector<byte_t> data(bytes.size());
    for (int i = 0; i < bytes.size(); i++) {
        data[i] = static_cast<unsigned char>(bytes[i]);
    }
    data.back().set_is_X();
    return calc_sha256(calc_sha256(data));
}

std::pair<uint32_t, std::string> find_best_hash(const std::string &bytes) {
    auto digest = build_digest(bytes);
    std::reverse(digest.begin(), digest.end());

    auto less_compare = [&](uint32_t lhs, uint32_t rhs) {
        uint32_t index = 0;
        for (; index < digest.size() && digest[index].dp[lhs] == digest[index].dp[rhs]; index++) {
        }
        return index < digest.size() && digest[index].dp[lhs] < digest[index].dp[rhs];
    };

    uint32_t best_x = 0;
    for (uint32_t x = 0; x < 256; x++) {
        if (less_compare(x, best_x)) {
            best_x = x;
        }
    }

    std::string hash;
    for (uint32_t i = 0; i < digest.size(); i++) {
        hash += digest[i].dp[best_x];
    }
    return {best_x, hash};
}