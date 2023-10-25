#include "miner.hpp"
#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <chrono>
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

bool Miner::available_good() const {
    return is_good;
}

void Miner::simulate_lock() {
    if (mutex == mutex_t::WAITED_FOR_LOCK) {
        mutex = mutex_t::LOCKED;
        while (mutex == mutex_t::LOCKED) {}
    }
}

int64_t Miner::hashrate() const {
    return hashrate_snapshot;
}

[[noreturn]] void Miner::run() {
    auto time_start = steady_clock::now();
    uint64_t count_of_hashes_calculated = 0;
    for (uint32_t nonce = 1; true; nonce++) {

        if (nonce % 0x1000 == 0) {
            simulate_lock();
        }

        std::string hash = b.calc_hash(nonce);
        count_of_hashes_calculated++;
        // ASSERT(b.calc_hash(nonce) == b.trivial_calc_hash(nonce), "failed");

        if (hash < best_block_hash) {
            best_block_hash = hash;
            best_block = b;
            is_good = hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0;// && hash[4] == 0;
            logger.print("NEW BEST BLOCK: " + bytes_to_hex(best_block_hash));
        }

        if (nonce % 0x1000 == 0) {
            auto time_stop = steady_clock::now();
            auto duration = time_stop - time_start;
            double time = duration_cast<nanoseconds>(duration).count() / 1e9;

            if (time > HASHRATE_UPDATE_TIME) {
                time_start = time_stop;

                int64_t hashrate = count_of_hashes_calculated / time;
                hashrate_snapshot = hashrate;
                count_of_hashes_calculated = 0;
                logger.print("HASHRATE: " + pretty_hashrate(hashrate));

                b.build_extranonce2();// продолжим с новым значением extranonce2
            }
        }
    }
}
