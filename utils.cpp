#include "utils.hpp"

std::mt19937 &get_random_engine() {
    static std::mt19937 rnd(42);
    return rnd;
}

uint32_t rnd() {
    return get_random_engine()();
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