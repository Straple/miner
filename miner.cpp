#include "miner.hpp"
#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <chrono>
#include <utility>

using namespace std::chrono;

void Miner::init(uint32_t ID, uint32_t miners_count, lite_block new_block) {
    id = ID;
    current_block = std::move(new_block);
    // logger = "worker" + std::to_string(ID) + ".txt";
    // logger = "worker" + std::to_string(mining_round) + "_" + std::to_string(ID) + ".txt";
    thread = std::thread(&Miner::run, this, miners_count);
}

uint32_t Miner::get_best_nonce() {
    // TODO: WARGNING
    // вообще тут так делать не очень, но да ладно
    // потому что майнер в это время может найти блок и сказать is_good
    is_good = false;
    return best_nonce;
}

bool Miner::available_good() const {
    return is_good;
}

int64_t Miner::get_hashrate() const {
    return hash_calculated_count / work_time;
}

Statistic &Miner::get_statistic() {
    return state;
}

bool Miner::is_done() const {
    return done;
}

void Miner::join() {
    thread.join();
}

void Miner::run(uint32_t miners_count) {
    auto time_start = steady_clock::now();

    for (uint64_t nonce = id; nonce < NONCE_BOUND; nonce += miners_count) {
        fast_string hash = current_block.calc_hash(nonce);
        ASSERT(hash.size() == 32, "bad hash size");

        if (state.add(nonce, hash)) {
            best_nonce = nonce;
            is_good = hash[31] == 0 && hash[30] == 0 && hash[29] == 0 && hash[28] == 0 && hash[27] == 0;
            // logger.print("NEW BEST BLOCK: ", bytes_to_hex(reverse_str(hash).to_str()));
        }
    }

    hash_calculated_count += NONCE_BOUND / miners_count;

    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    work_time = duration_cast<nanoseconds>(duration).count() / 1e9;

    done = true;
}
