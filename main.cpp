#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include "block.hpp"
using namespace std::chrono;

#include "bits_manipulation.hpp"

std::string pretty_hashrate(uint64_t hashrate) {
    auto unit = [&]() {
        if (hashrate < 1e3) {
            return "";
        } else if (hashrate < 1e6) {
            hashrate /= 1e3;
            return "K";
        } else if (hashrate < 1e9) {
            hashrate /= 1e6;
            return "M";
        } else if (hashrate < 1e12) {
            hashrate /= 1e9;
            return "G";
        } else if (hashrate < 1e15) {
            hashrate /= 1e12;
            return "T";
        }
    };
    std::string s = unit();
    return std::to_string(hashrate) + s + "H/s";
}

int main() {
    block b = {
        2,
        "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717",
        "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a",
        0x53058b35,
        0x19015f53,
        856192328};

    /*uint64_t nonce = 0x01f3a5c78;
    std::cout << uint32_t(-1) << '\n';
    std::cout << nonce << '\n';
    std::cout << integer_to_hex(nonce, 8) << '\n';
    std::cout << byte_reverse_in_hex(integer_to_hex(nonce, 8)) << '\n';
    std::cout << reverse_bytes(nonce) << '\n';
    std::cout << reverse_str(integer_to_bytes(reverse_bytes(nonce), 8)) << '\n';
    std::cout << hex_to_bytes(byte_reverse_in_hex(integer_to_hex(nonce, 8)))
              << '\n';
    return 0;*/

    b.build_data();

    std::cout << "answer: " << b.calc_hash(856192328) << '\n';

    auto start = steady_clock::now();

    const uint64_t N = 0x100000000;

    uint64_t nonce = 856192328;
    std::string target = b.calc_target();
    std::cout << "target: " << target << '\n';
    while (nonce < N) {
        if (nonce % 0x001000000 == 0) {
            std::cout << "progress: " << nonce * 1.0 / N * 100 << "%"
                      << std::endl;
        }
        if (b.calc_hash(nonce) < target) {
            std::cout << nonce << ": " << bytes_to_hex(b.calc_hash(nonce))
                      << std::endl;
            // break;
        }
        nonce++;
    }

    auto stop = steady_clock::now();
    auto duration = stop - start;

    // Nvidia 3050: 10.63 MH/s
    // me: 952H/s -> 57444H/s -> 62398H/s -> 4MH/s

    // 800s перебрал весь nonce

    int64_t hashrate =
        1.0 * N / (duration_cast<nanoseconds>(duration).count() / 1e9);
    std::cout << pretty_hashrate(hashrate) << "\n";
    std::cout << "time calculating: "
              << duration_cast<nanoseconds>(duration).count() / 1e9 << "s\n";
}