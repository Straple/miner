#include <chrono>
#include <iostream>
#include <vector>

using namespace std::chrono;

#include "bits_manipulation.hpp"
#include "constants.hpp"
#include "miner.hpp"
#include "pool_client.hpp"

int main(int argc, char **argv) {
    int MINERS = 4;
    if (argc == 2) {
        MINERS = std::atoi(argv[1]);
        std::cout << "Miners count: " << MINERS << '\n';
    }

    // connect to pool
    PoolClient pool_client;

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

    Logger miners_statistic("workers_statistic.txt");
    auto time_start = steady_clock::now();
    std::string best_block_hash = "z";

    while (true) {
        for (uint32_t id = 0; id < MINERS; id++) {
            if (miners[id].available_good()) {
                block b = miners[id].get_best_block();
                pool_client.submit(b);
                std::string hash = b.calc_hash(b.nonce);
                if (hash < best_block_hash) {
                    best_block_hash = std::move(hash);
                    miners_statistic.print("NEW BEST BLOCK: " + bytes_to_hex(best_block_hash));
                }
            }
        }

        // посмотрит, есть ли там что-то для чтения
        if (pool_client.reading_is_available()) {
            // если есть,
            // то мы это считаем и посмотрим
            bool new_miner_task = false;
            block new_block = pool_client.get_new_block(new_miner_task);

            if (new_miner_task) {
                for (uint32_t id = 0; id < MINERS; id++) {
                    miners[id].set_new_block(new_block);
                }
            }
        }

        {
            auto time_stop = steady_clock::now();
            auto duration = time_stop - time_start;
            double time = duration_cast<nanoseconds>(duration).count() / 1e9;

            if (time > HASHRATE_UPDATE_TIME) {
                time_start = time_stop;

                int64_t hashrate = 0;
                for (uint32_t id = 0; id < MINERS; id++) {
                    hashrate += miners[id].hashrate();
                }

                miners_statistic.print("HASHRATE: " + pretty_hashrate(hashrate));
            }
        }

        // чтобы постоянно не крутиться, так как мы так только будем мешать воркерам
        std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_THREAD_SLEEP_TIME_MS));
    }
}
