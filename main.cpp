#include <iostream>
#include <string>
#include <vector>

#include "bits_manipulation.hpp"
#include "miner.hpp"
#include "pool_client.hpp"
#include "utils.hpp"

int main() {

    // connect to pool
    PoolClient pool_client;

    // start miners
    const uint32_t MINERS = 2;
    std::vector<Miner> miners(MINERS);
    {
        bool new_miner_task;
        block b = pool_client.get_new_block(new_miner_task);
        for (uint32_t id = 0; id < MINERS; id++) {
            miners[id].Init(id, b);
        }
    }

    while (true) {
        for (uint32_t id = 0; id < MINERS; id++) {
            block b = miners[id].get_best_block();
            std::string hash = b.calc_hash(b.nonce);
            if (hash[0] == 0 && hash[1] == 0) {
                std::cout << "FIND NICE <" << id << "> " << b.nonce << ": " << bytes_to_hex(hash) << std::endl;
                pool_client.submit(b);
            }
        }

        bool new_miner_task;
        block new_block = pool_client.get_new_block(new_miner_task);
        if (new_miner_task) {
            std::cout << "DETECTED NEW BLOCK" << std::endl;
            for (uint32_t id = 0; id < MINERS; id++) {
                miners[id].set_new_block(new_block);
            }
        }
    }
}
