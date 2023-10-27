#include <chrono>
#include <iostream>
#include <vector>

using namespace std::chrono;

#include "bits_manipulation.hpp"
#include "constants.hpp"
#include "miner.hpp"
#include "pool_client.hpp"

// reading from blocks dataset
// [80 bytes of best block bytes data, statistic]
std::pair<fast_string, Statistic> read_block_data(std::istream &input) {
    std::string tmp;
    input >> tmp >> tmp;// skip "[2023-10-27 16:21:36]"

    std::string best_hash;
    input >> best_hash;

    std::string block_bytes_data;
    input >> block_bytes_data;

    std::cout << "best_hash: " << best_hash << std::endl;
    std::cout << "block_bytes_data: " << block_bytes_data << std::endl;

    input >> tmp;
    ASSERT(tmp == "COUNTS:", "invalid input");

    Statistic state;
    for (uint32_t index = 0; index < state.LEN; index++) {
        input >> state.counts[index];
        //std::cout << state.counts[index] << ' ';
    }
    //std::cout << std::endl;

    input >> tmp;
    ASSERT(tmp == "EXEMPLARS:", "invalid input");

    for (uint32_t index = 0; index < state.LEN; index++) {
        input >> state.exemplar[index];
        //std::cout << state.exemplar[index] << ' ';
    }
    //std::cout << std::endl;

    input >> tmp;
    ASSERT(tmp == "EXEMPLARS_HASH:", "invalid input");

    for (uint32_t index = 0; index < state.LEN; index++) {
        input >> tmp;
        state.exemplar_hash[index] = reverse_str(hex_to_bytes(tmp));
        //std::cout << state.exemplar_hash[index] << ' ';
    }
    //std::cout << std::endl;

    for (int index = state.LEN - 1; index >= 0; index--) {
        if (state.counts[index] != 0) {
            state.best_index = index;
            break;
        }
    }

    // bytes_to_hex(current_block.save_bytes_data.to_str());

    lite_block b(hex_to_bytes(block_bytes_data));

    // std::cout << bytes_to_hex(reverse_str(b.calc_hash(state.get_best().first))) << std::endl;
    // std::cout << bytes_to_hex(reverse_str(state.get_best().second).to_str()) << std::endl;
    ASSERT(b.calc_hash(state.get_best().first) == state.get_best().second, "invalid bytes data");

    return {hex_to_bytes(block_bytes_data), state};
}

int main(int argc, char **argv) {
    /*std::ifstream input("blocks_dataset.txt");
    auto [block_bytes_data, state] = read_block_data(input);
    std::cout << "\n\n";
    std::cout << bytes_to_hex(block_bytes_data.to_str()) << std::endl;
    return 0;*/
    /*fast_string s(2);
    s[0] = 0b00110000;
    std::cout << s.builtin_ctz() << std::endl;
    return 0;*/
    /*for(int p = 1; p < 16; p++){
        std::cout << p << ' ';
        std::string lol = "0000";
        lol[3] = p < 10 ? (p + '0') : (p - 10 + 'a');
        std::cout << lol << ' ' << uint32_t(uint8_t(hex_to_bytes(lol)[0])) << ' ';
        fast_string s = reverse_str(hex_to_bytes(lol));
        std::cout << s.builtin_ctz() << ' ';

        fast_string hash = reverse_str(s);

        uint32_t x = 0;
        for (; x < 32 && hash[x] == 0; x++) {
        }
        int bit = 7;
        for (; bit >= 0 && ((uint8_t(hash[x]) >> bit) & 1) == 0; bit--) {
        }
        //counts[x * 8 + (7 - bit)]++;
        std::cout << x * 8 + (7 - bit) << '\n';
    }*/

    //uint32_t x = 0xa00ab0c1;
    //std::cout << bytes_to_hex(integer_to_bytes(x, 4)) << '\n';

    // bytes_to_hex(reverse_str(best_block_hash).to_str())
    //return 0;

    //===============================================================

    /*fast_string lhs(80);
    fast_string rhs(80);
    for (int i = 0; i < 80; i++) {
        lhs[i] = 0;
        rhs[i] = i < 60;
    }

    std::cout << (lhs < rhs) << std::endl;

    for (int x = 0; x < (1 << 6); x++) {

        char a = (x & 1) + '0';
        char b = ((x >> 1) & 1) + '0';
        char c = ((x >> 2) & 1) + '0';

        char d = ((x >> 3) & 1) + '0';
        char e = ((x >> 4) & 1) + '0';
        char f = ((x >> 5) & 1) + '0';
        std::string lhs;
        lhs += a;
        lhs += b;
        lhs += c;
        std::string rhs;
        rhs += d;
        rhs += e;
        rhs += f;
        std::cout << lhs << ' ' << rhs << ' ' << (fast_string(lhs) < fast_string(rhs)) << std::endl;
    }*/
    //00 00 0
    //10 00 0
    //01 00 0
    //11 00 0
    //00 10 1
    //10 10 0
    //01 10 0
    //11 10 0
    //00 01 1
    //10 01 1
    //01 01 0
    //11 01 0
    //00 11 1
    //10 11 1
    //01 11 1
    //11 11 0
    //return 0;
    uint32_t threads_count = 2;
    if (argc == 2) {
        threads_count = std::atoi(argv[1]);
    }
    std::cout << "Threads count: " << threads_count << std::endl;

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
    std::cerr << bytes_to_hex(reverse_str(lite_block.calc_hash(nonce)).to_str()) << std::endl;
    current_block.build_data();
    std::cerr << bytes_to_hex(reverse_str(current_block.trivial_calc_hash(nonce)).to_str()) << std::endl;
    std::cerr << bytes_to_hex(reverse_str(current_block.calc_hash(nonce)).to_str()) << std::endl;


    std::cerr << integer_to_hex(nonce, 8) << std::endl;
    std::cerr << nonce << std::endl;
    //std::cerr << lite_block.get_nonce() << std::endl;
    std::cerr.flush();

    ASSERT(current_block.calc_hash(nonce) == lite_block.calc_hash(nonce), "invalid hash");
*/
    //return 0;

    Logger miners_statistic("miners_statistic.txt");
    Logger blocks_dataset("blocks_dataset.txt");

    auto time_start = steady_clock::now();

    uint64_t mining_round_count = 0;

    auto merge_statistic = [&](std::vector<Miner> &miners) {
        // соберем статистику
        Statistic state;
        for (uint32_t id = 0; id < threads_count; id++) {
            state.add(miners[id].get_statistic());
            /*uint32_t nonce = miners[id].get_best_nonce();
            fast_string hash = current_block.calc_hash(nonce);
            ASSERT(current_block.calc_hash(nonce) == current_block.trivial_calc_hash(nonce), "invalid hash");
            ASSERT(current_block.calc_hash(nonce) == miners[id].current_block.calc_hash(nonce), "invalid hash");
            if (!(current_block.calc_hash(nonce) == miners[id].best_block_hash)) {
                std::cout << current_block.calc_hash(nonce) << std::endl;
                std::cout << miners[id].best_block_hash << std::endl;
            }
            ASSERT(current_block.calc_hash(nonce) == miners[id].best_block_hash, "invalid hash");*/
        }
        return state;
    };

    // NOLINTNEXTLINE
    auto mining = [&]() {
        auto time_start = steady_clock::now();

        mining_round_count++;
        miners_statistic.print("Start mining #" + std::to_string(mining_round_count));

        current_block.build_extranonce2();// строим рандомный extranonce2

        // запускаем майнеров
        std::vector<Miner> miners(threads_count);
        ::lite_block current_lite_block = current_block.build_lite_block();
        for (uint32_t id = 0; id < threads_count; id++) {
            miners[id].init(id, threads_count, current_lite_block);
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
                    fast_string hash = current_block.calc_hash(nonce);
                    pool_client.submit(current_block);
                    miners_statistic.print("SUBMIT BLOCK: ", bytes_to_hex(reverse_str(hash)));
                }
            }

            // чтобы постоянно не крутиться, так как мы так только будем мешать воркерам
            std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_THREAD_SLEEP_TIME_MS));
        }

        for (uint32_t id = 0; id < threads_count; id++) {
            miners[id].join();
        }

        Statistic state = merge_statistic(miners);

        auto [best_nonce, best_hash] = state.get_best();

        ASSERT(best_hash == current_block.calc_hash(best_nonce), "invalid best hash");
        current_block.calc_hash(best_nonce);// нужно обязательно вычислить еще раз, чтобы обновить nonce на best_nonce
        blocks_dataset.print(bytes_to_hex(reverse_str(best_hash).to_str()), ' ', bytes_to_hex(current_block.save_bytes_data.to_str()), "\n", state, "\n");

        {
            int64_t hashrate_sum = 0;
            for (uint32_t id = 0; id < threads_count; id++) {
                hashrate_sum += miners[id].get_hashrate();
            }
            std::stringstream ss;
            ss << "Hashrate: " << pretty_hashrate(hashrate_sum) << " | ";
            for (uint32_t id = 0; id < threads_count; id++) {
                ss << pretty_hashrate(miners[id].get_hashrate()) << ' ';
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
        //return 0;
    }
}
