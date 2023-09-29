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
        data[i] = byte_t(bytes[i]);
    }
    for(int x = 0; x < 8; x++){
        data.back()[x] = bitsolver(x);
    }
    return calc_sha256(calc_sha256(data));
}

#include<iostream>

std::pair<uint32_t, std::string> find_best_hash(const std::string &bytes) {
    auto digest = build_digest(bytes);
    std::reverse(digest.begin(), digest.end());

    auto less_compare = [&](uint32_t lhs, uint32_t rhs) {
        uint32_t index = 0;
        for (; index < digest.size() && digest[index].calc(lhs) == digest[index].calc(rhs); index++) {
        }
        return index < digest.size() && digest[index].calc(lhs) < digest[index].calc(rhs);
    };

    uint32_t best_x = 0;
    for (uint32_t x = 0; x < (1<<8); x++) {
        if (less_compare(x, best_x)) {
            best_x = x;
        }
    }

    //std::cout << digest[0] << '\n';
    //exit(0);

    std::string hash(digest.size(), 0);
    for (uint32_t i = 0; i < digest.size(); i++) {
        hash[i] = digest[i].calc(best_x);
    }
    return {best_x, hash};
}
