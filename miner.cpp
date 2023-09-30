#include "miner.hpp"
#include "bits_manipulation.hpp"
#include "utils.hpp"
#include <chrono>
#include <iostream>

using namespace std::chrono;

void Miner::set_new_block(block new_block_) {
    std::unique_lock locker{m};
    std::cout << "SET NEW BLOCK FOR THREAD " << id << std::endl;
    best_block_hash = "z";
    best_block_is_submited = false;
    is_new_block = true;
    new_block = std::move(new_block_);
}

void Miner::keep_new_block() {
    std::unique_lock locker{m};
    if (is_new_block) {
        is_new_block = false;
        b = std::move(new_block);
        std::cout << "KEEP NEW BLOCK " << id << std::endl;
    }
}

void Miner::set_best_block(const std::string &hash) {
    std::unique_lock locker{m};
    std::cout << "NEW BEST " << id << ": " << bytes_to_hex(hash) << std::endl;
    best_block = b;
    best_block_hash = hash;
    best_block_is_submited = false;
}

block Miner::get_best_block() {
    std::unique_lock locker{m};
    best_block_is_submited = true;
    return best_block;
}

void Miner::run() {
    auto start = steady_clock::now();
    for (uint32_t nonce = 0; true; nonce++) {
        if (nonce % 0x10000 == 0) {
            keep_new_block();
        }
        std::string hash = b.calc_hash(nonce);
        //ASSERT(b.calc_hash(nonce) == b.trivial_calc_hash(nonce), "failed");

        if (hash < best_block_hash) {
            set_best_block(hash);
        }

        if (nonce % 40'000'000 == 0) {
            auto stop = steady_clock::now();
            auto duration = stop - start;
            start = stop;

            int64_t hashrate =
                    1.0 * 40'000'000 / (duration_cast<nanoseconds>(duration).count() / 1e9);
            std::cout << "Worker " << id << " " << pretty_hashrate(hashrate) << std::endl;

            uint32_t extranonce2 = rnd();
            b.extranonce2 = integer_to_hex(extranonce2, b.extranonce2_size * 2);
            b.build_data();
        }
    }
}
