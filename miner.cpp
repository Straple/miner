#include "miner.hpp"
#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <chrono>
#include <utility>

using namespace std::chrono;

void Miner::init(uint32_t ID, lite_block new_block, uint32_t mining_round, std::vector<std::atomic<bool>> &visited) {
    id = ID;
    current_block = std::move(new_block);
    logger = "worker" + std::to_string(mining_round) + "_" + std::to_string(ID) + "_" + ".txt";
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

int64_t Miner::hashrate() const {
    return hash_calculated_count / work_time;
}

bool Miner::is_done() const {
    return done;
}

void Miner::join() {
    thread.join();
}

void Miner::run(std::vector<std::atomic<bool>> &visited) {
    auto time_start = steady_clock::now();

    auto do_task = [&](uint32_t left, uint32_t right) {
        //ASSERT(left <= right, "bad borders");
        for (uint32_t nonce = left; nonce <= right; nonce++) {
            std::string hash = current_block.calc_hash(nonce);
            if (hash < best_block_hash) {
                best_block_hash = hash;
                best_nonce = current_block.get_nonce();
                is_good = hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0 && hash[4] == 0;
                logger.print("NEW BEST BLOCK: " + bytes_to_hex(best_block_hash));
            }
        }
        hash_calculated_count += right - left + 1;
    };

    for (uint32_t block_index = 0; block_index < TASK_BLOCKS_COUNT; block_index++) {
        bool expected = false;
        if (visited[block_index].compare_exchange_strong(expected, true)) {
            uint32_t left = block_index * TASK_BLOCK_LEN;
            uint32_t right = left + TASK_BLOCK_LEN - 1;

            // logger.print(std::to_string(block_index) + ' ' + std::to_string(left) + ' ' + std::to_string(right));

            do_task(left, right);
        }
    }

    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    work_time = duration_cast<nanoseconds>(duration).count() / 1e9;

    done = true;
}
