#include <iostream>
#include <vector>

#include "miner.hpp"
#include "pool_client.hpp"

int main() {
    // connect to pool
    PoolClient pool_client;

    const uint32_t MINERS = 1;
    std::vector<Miner> miners(MINERS);
    std::cout << "START MINERS...\n";
    {
        bool new_miner_task = false;
        block b = pool_client.get_new_block(new_miner_task);
        for (uint32_t id = 0; id < MINERS; id++) {
            miners[id].Init(id, b);
        }
    }
    std::cout << "OK\n\n";

    while (true) {
        for (uint32_t id = 0; id < MINERS; id++) {
            if (miners[id].available_good()) {
                pool_client.submit(miners[id].get_best_block());
            }
        }

        if (pool_client.reading_is_available()) {
            // OK довольно быстро 0.000119976s, но бывает чуть дольше
            //auto time_start = steady_clock::now();
            bool new_miner_task = false;
            block new_block = pool_client.get_new_block(new_miner_task);
            //auto time_stop = steady_clock::now();
            //auto duration = time_stop - time_start;
            //std::cout << "WAITING FOR READ BLOCK: " << (duration_cast<nanoseconds>(duration).count() / 1e9) << std::endl;

            if (new_miner_task) {
                for (uint32_t id = 0; id < MINERS; id++) {
                    miners[id].set_new_block(new_block);
                }
            }
        }

        // OK 10ms
        //auto time_start = steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //auto time_stop = steady_clock::now();
        //auto duration = time_stop - time_start;
        //std::cout << "SLEEP: " << (duration_cast<nanoseconds>(duration).count() / 1e9) << std::endl;
    }
}
