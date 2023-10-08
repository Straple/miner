#include "miner.hpp"
#include "bits_manipulation.hpp"
#include "utils.hpp"
#include <chrono>
#include <iostream>
#include <utility>

using namespace std::chrono;

void Miner::Init(uint32_t ID, block new_block) {
    id = ID;
    b = std::move(new_block);
    b.build_extranonce2();
    logger = "worker" + std::to_string(ID) + ".txt";
    thread = std::thread(&Miner::run, this);
}

void Miner::set_new_block(block new_block) {
    auto time_start = steady_clock::now();

    mutex = mutex_t::WAITED_FOR_LOCK;
    while (mutex != mutex_t::LOCKED) {}

    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    logger.print("SET NEW BLOCK, waited time: " + std::to_string(duration_cast<nanoseconds>(duration).count() / 1e9) + "s");
    b = std::move(new_block);
    b.build_extranonce2();

    mutex = mutex_t::UNLOCKED;
}

block Miner::get_best_block() {
    auto time_start = steady_clock::now();

    mutex = mutex_t::WAITED_FOR_LOCK;
    while (mutex == mutex_t::WAITED_FOR_LOCK) {}

    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    logger.print("GET BEST BLOCK, waited time: " + std::to_string(duration_cast<nanoseconds>(duration).count() / 1e9) + "s");

    block copy_best_block = best_block;
    is_good = false;// теперь нам незачем этот гуд

    mutex = mutex_t::UNLOCKED;
    return copy_best_block;
}

bool Miner::available_good() {
    return is_good;
}

[[noreturn]] void Miner::run() {
    auto time_start = steady_clock::now();
    for (uint32_t nonce = 1; true; nonce++) {

        if (nonce % 0x1000 == 0 && mutex == mutex_t::WAITED_FOR_LOCK) {
            mutex = mutex_t::LOCKED;
            while (mutex == mutex_t::LOCKED) {}
        }

        std::string hash = b.calc_hash(nonce);
        //ASSERT(b.calc_hash(nonce) == b.trivial_calc_hash(nonce), "failed");

        if (hash < best_block_hash) {
            best_block_hash = hash;
            best_block = b;
            is_good = hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0 && hash[4] == 0;
            logger.print("NEW BEST BLOCK: " + bytes_to_hex(best_block_hash));
        }

        if (nonce % 40'000'000 == 0) {
            auto time_stop = steady_clock::now();
            auto duration = time_stop - time_start;
            time_start = time_stop;

            int64_t hashrate =
                    1.0 * 40'000'000 / (duration_cast<nanoseconds>(duration).count() / 1e9);
            logger.print("HASHRATE: " +  pretty_hashrate(hashrate));

            b.build_extranonce2();// продолжим с новым значением extranonce2
        }
    }
}
