#include <chrono>
#include <iostream>
#include <vector>

using namespace std::chrono;

#include "bits_manipulation.hpp"
#include "constants.hpp"
#include "miner.hpp"
#include "pool_client.hpp"

int main(int argc, char **argv) {
    uint32_t threads_count = 2;
    if (argc == 2) {
        threads_count = std::atoi(argv[1]);
        std::cout << "Threads count: " << threads_count << '\n';
    }

    // connect to pool
    PoolClient pool_client;

    bool new_miner_task = false;
    block current_block = pool_client.get_new_block(new_miner_task);

    // EXAMPLE TO USE LITE BLOCK
    /*current_block.build_extranonce2();

    uint32_t nonce = 0x1cabf938;

    //current_block.calc_hash(nonce);

    // babb95b7a797b2e17dbc71c7b49dce0c15687d7704c03a4394fdeb40eaadc31c

    lite_block lite_block = current_block.build_lite_block();
    std::cout << bytes_to_hex(lite_block.calc_hash(nonce)) << '\n';
    current_block.build_data();
    std::cout << bytes_to_hex(current_block.trivial_calc_hash(nonce)) << '\n';
    std::cout << bytes_to_hex(current_block.calc_hash(nonce)) << '\n';


    std::cout << integer_to_hex(nonce, 8) << '\n';
    std::cout << nonce << '\n';
    std::cout << lite_block.get_nonce() << '\n';

    ASSERT(current_block.calc_hash(nonce) == lite_block.calc_hash(nonce), "invalid hash");

    return 0;*/

    Logger miners_statistic("miners_statistic.txt");
    Logger blocks_dataset("blocks_dataset.txt");

    auto time_start = steady_clock::now();

    uint64_t mining_round_count = 0;

    auto mining = [&]() {
        auto time_start = steady_clock::now();

        mining_round_count++;
        miners_statistic.print("Start mining #" + std::to_string(mining_round_count));

        current_block.build_extranonce2();// строим рандомный extranonce2

        // запускаем майнеров
        std::vector<std::atomic<bool>> visited(TASK_BLOCKS_COUNT);
        std::vector<Miner> miners(threads_count);
        ::lite_block current_lite_block = current_block.build_lite_block();
        for (uint32_t id = 0; id < threads_count; id++) {
            miners[id].init(id, current_lite_block, mining_round_count, visited);
        }

        miners_statistic.print("OK. Waiting...");

        auto all_miners_are_done = [&]() {
            for (uint32_t id = 0; id < threads_count; id++) {
                if (!miners[id].is_done()) {
                    return false;
                }
            }
            return true;
        };

        while (!all_miners_are_done()) {
            // проверяем, вдруг есть ответ
            for (uint32_t id = 0; id < threads_count; id++) {
                if (miners[id].available_good()) {
                    uint32_t nonce = miners[id].get_best_nonce();
                    std::string hash = current_block.calc_hash(nonce);
                    pool_client.submit(current_block);
                    miners_statistic.print("SUBMIT BLOCK: " + bytes_to_hex(hash));
                }
            }

            // чтобы постоянно не крутиться, так как мы так только будем мешать воркерам
            std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_THREAD_SLEEP_TIME_MS));
        }

        for (uint32_t id = 0; id < threads_count; id++) {
            miners[id].join();
        }

        std::string best_block_hash = "z";
        uint32_t best_nonce = -1;
        for (uint32_t id = 0; id < threads_count; id++) {
            uint32_t nonce = miners[id].get_best_nonce();
            std::string hash = current_block.calc_hash(nonce);
            ASSERT(current_block.calc_hash(nonce) == miners[id].current_block.calc_hash(nonce), "invalid hash");
            ASSERT(current_block.calc_hash(nonce) == miners[id].best_block_hash, "invalid hash");
            ASSERT(current_block.calc_hash(nonce) == current_block.trivial_calc_hash(nonce), "invalid hash");
            if (hash < best_block_hash) {
                best_block_hash = hash;
                best_nonce = nonce;
            }
        }

        current_block.calc_hash(best_nonce);// нужно обязательно вычислить еще раз, чтобы обновить nonce на best_nonce
        ASSERT(current_block.calc_hash(best_nonce) == best_block_hash, "invalid best_block_hash");

        blocks_dataset.print(bytes_to_hex(current_block.save_bytes_data) + " " + bytes_to_hex(best_block_hash));

        {
            int64_t hashrate_sum = 0;
            for (uint32_t id = 0; id < threads_count; id++) {
                hashrate_sum += miners[id].hashrate();
            }
            std::stringstream ss;
            ss << "Hashrate: " << pretty_hashrate(hashrate_sum) << " | ";
            for (uint32_t id = 0; id < threads_count; id++) {
                ss << pretty_hashrate(miners[id].hashrate()) << ' ';
            }
            miners_statistic.print(ss.str());
        }

        auto time_stop = steady_clock::now();
        auto duration = time_stop - time_start;
        double time = duration_cast<nanoseconds>(duration).count() / 1e9;

        miners_statistic.print("Time: " + std::to_string(time) + 's');

        miners_statistic.print("Done.\n");
    };

    while (true) {

        pool_client.update_connection();

        // посмотрит, есть ли там что-то для чтения
        if (pool_client.reading_is_available()) {
            // если есть,
            // то мы это считаем и посмотрим
            bool new_miner_task = false;
            current_block = pool_client.get_new_block(new_miner_task);
        }

        mining();
    }
}
