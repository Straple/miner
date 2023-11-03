#include "utils.hpp"
#include "assert.hpp"

std::mt19937 &get_random_engine() {
    static std::mt19937 rnd(42);
    return rnd;
}

void set_seed_random_engine(uint64_t seed) {
    get_random_engine() = std::mt19937(seed);
}

uint32_t rnd() {
    static std::uniform_int_distribution<uint32_t> distrib(0, UINT32_MAX);
    return distrib(get_random_engine());
}

int rnd(int left, int right) {
    ASSERT(left <= right, "bad borders");
    return static_cast<int>(rnd() % static_cast<uint32_t>(right - left + 1)) + left;
}

double rnd_chance() {
    return static_cast<double>(rnd()) / UINT32_MAX;
}

std::string pretty_hashrate(uint64_t hashrate) {
    std::string prefix;
    if (hashrate < 1e3) {

    } else if (hashrate < 1e6) {
        hashrate /= 1e2;
        prefix = "K";
    } else if (hashrate < 1e9) {
        hashrate /= 1e5;
        prefix = "M";
    } else if (hashrate < 1e12) {
        hashrate /= 1e8;
        prefix = "G";
    } else if (hashrate < 1e15) {
        hashrate /= 1e11;
        prefix = "T";
    }
    return std::to_string(hashrate / 10) + "." + std::to_string(hashrate % 10) + prefix + "H/s";
}