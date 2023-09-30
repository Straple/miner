#include <iostream>
#include <string>
#include <vector>

#include "bits_manipulation.hpp"
#include "miner.hpp"
#include "pool_client.hpp"
#include "utils.hpp"

int main() {

    PoolClient pool_client;

    const uint32_t WORKERS = 2;
    std::vector<Miner> workers(WORKERS);
    for (uint32_t id = 0; id < WORKERS; id++) {
        workers[id].id = id;
        bool is_new_block;
        workers[id].set_new_block(pool_client.get_new_block(is_new_block));
        workers[id].thr = std::thread(&Miner::run, &workers[id]);
    }

    while (true) {
        for (uint32_t id = 0; id < WORKERS; id++) {
            if (!workers[id].best_block_is_submited) {
                std::string hash = workers[id].best_block_hash;
                if (hash[0] == 0 && hash[1] == 0 && hash[2] == 0) {
                    block b = workers[id].get_best_block();
                    std::cout << "FIND NICE <" << id << "> " << b.nonce << ": " << bytes_to_hex(hash) << std::endl;
                    pool_client.submit(b);
                }
            }
        }

        bool new_miner_task;
        block new_block = pool_client.get_new_block(new_miner_task);
        if (new_miner_task) {
            std::cout << "DETECTED NEW BLOCK" << std::endl;
            for (uint32_t id = 0; id < WORKERS; id++) {
                // choose extranonce2
                uint32_t extranonce2 = rnd();
                new_block.extranonce2 = integer_to_hex(extranonce2, new_block.extranonce2_size * 2);
                new_block.build_data();

                workers[id].set_new_block(new_block);
            }
        }
    }
}
