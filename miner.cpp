#include "miner.hpp"
#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <chrono>
#include <utility>

using namespace std::chrono;

void Miner::init(lite_block new_block, std::vector<std::atomic<bool>> &visited) {
    current_block = new_block;
    // logger = "worker" + std::to_string(ID) + ".txt";
    // logger = "worker" + std::to_string(mining_round) + "_" + std::to_string(ID) + ".txt";
    thread = std::thread(&Miner::run, this, std::ref(visited));
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

void Miner::run(std::vector<std::atomic<bool>> &visited) {
    auto time_start = steady_clock::now();

    auto do_task = [&](uint64_t left, uint64_t right) {
        ASSERT(left <= right, "bad borders");
        for (uint64_t nonce = left; nonce <= right; nonce++) {
            fast_string hash = current_block.calc_hash(nonce);

            if (state.add(nonce, hash)) {
                best_nonce = nonce;
                is_good = hash[31] == 0 && hash[30] == 0 && hash[29] == 0 && hash[28] == 0 && hash[27] == 0;
                // logger.print("NEW BEST BLOCK: ", bytes_to_hex(reverse_str(hash).to_str()));
            }
        }
        hash_calculated_count += right - left + 1;
    };

    for (uint64_t block_index = 0; block_index < TASK_BLOCKS_COUNT; block_index++) {
        bool expected = false;
        if (visited[block_index].compare_exchange_strong(expected, true)) {
            uint64_t left = block_index * TASK_BLOCK_LEN;
            uint64_t right = left + TASK_BLOCK_LEN - 1;

            // logger.print(std::to_string(block_index) + ' ' + std::to_string(left) + ' ' + std::to_string(right));

            do_task(left, right);
        }
    }

    /*for (uint64_t nonce = id; nonce < NONCE_BOUND; nonce += miners_count) {
        fast_string hash = current_block.calc_hash(nonce);
        ASSERT(hash.size() == 32, "bad hash size");

        if (state.add(nonce, hash)) {
            best_nonce = nonce;
            is_good = hash[31] == 0 && hash[30] == 0 && hash[29] == 0 && hash[28] == 0 && hash[27] == 0;
            // logger.print("NEW BEST BLOCK: ", bytes_to_hex(reverse_str(hash).to_str()));
        }
    }

    hash_calculated_count += NONCE_BOUND / miners_count;*/

    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    work_time = duration_cast<nanoseconds>(duration).count() / 1e9;

    done = true;
}
