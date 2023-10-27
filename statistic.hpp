#pragma once

#include "block.hpp"
#include "fast_string.hpp"
#include <vector>

class Statistic {
    inline const static uint32_t LEN = 256;
    std::vector<uint32_t> counts = std::vector<uint32_t>(LEN);
    std::vector<fast_string> exemplar_hash = std::vector<fast_string>(LEN, std::string("\1"));
    std::vector<uint32_t> exemplar = std::vector<uint32_t>(LEN);
    uint32_t best_index = 0;

public:
    bool add(uint32_t nonce, const fast_string &hash);

    void add(const Statistic &other);

    [[nodiscard]] std::pair<uint32_t, fast_string> get_best() const;

    friend std::ostream &operator<<(std::ostream &output, const Statistic &state);

    friend std::pair<fast_string, Statistic> read_block_data(std::istream &input);
};
