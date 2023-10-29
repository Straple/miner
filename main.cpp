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

    // std::cout << "best_hash: " << best_hash << std::endl;
    // std::cout << "block_bytes_data: " << block_bytes_data << std::endl;

    input >> tmp;
    ASSERT(tmp == "COUNTS:", "invalid input");

    uint64_t count_sum = 0;
    Statistic state;
    for (uint32_t index = 0; index < state.LEN; index++) {
        input >> state.counts[index];
        //std::cout << state.counts[index] << ' ';
        count_sum += state.counts[index];
    }
    ASSERT(count_sum == NONCE_BOUND, "invalid counts");
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

void run_miner(int argc, char **argv) {
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

    //bool new_miner_task = false;
    block current_block;// = pool_client.get_new_block(new_miner_task);

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
        ASSERT(state.get_sum_count() == NONCE_BOUND, "FATAL");
        return state;
    };

    auto join_miners = [&](std::vector<Miner> &miners) {
        for (uint32_t id = 0; id < threads_count; id++) {
            miners[id].join();
        }
    };

    auto try_to_submit = [&](std::vector<Miner> &miners) {
        for (uint32_t id = 0; id < threads_count; id++) {
            if (miners[id].available_good()) {
                uint32_t nonce = miners[id].get_best_nonce();
                fast_string hash = current_block.calc_hash(nonce);
                pool_client.submit(current_block);
                miners_statistic.print("SUBMIT BLOCK: ", bytes_to_hex(reverse_str(hash)));
            }
        }
    };

    auto be_ready_for_submit = [&](std::vector<Miner> &miners) {
        auto all_miners_are_done = [&]() {
            for (uint32_t id = 0; id < threads_count; id++) {
                if (!miners[id].is_done()) {
                    return false;
                }
            }
            return true;
        };

        while (!all_miners_are_done()) {
            try_to_submit(miners);

            // чтобы постоянно не крутиться, так как мы так только будем мешать майнерам
            std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_THREAD_SLEEP_TIME_MS));
        }
        try_to_submit(miners);
    };

    auto print_hashrate = [&](std::vector<Miner> &miners) {
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
    };

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
            miners[id].init(current_lite_block, visited);
        }

        miners_statistic.print("OK. Waiting...");

        be_ready_for_submit(miners);

        join_miners(miners);

        Statistic state = merge_statistic(miners);
        auto [best_nonce, best_hash] = state.get_best();

        ASSERT(best_hash == current_block.calc_hash(best_nonce), "invalid best hash");
        current_block.calc_hash(best_nonce);// нужно обязательно вычислить еще раз, чтобы обновить nonce на best_nonce
        blocks_dataset.print(bytes_to_hex(reverse_str(best_hash).to_str()), ' ', bytes_to_hex(current_block.save_bytes_data.to_str()), "\n", state, "\n");

        print_hashrate(miners);

        auto time_stop = steady_clock::now();
        auto duration = time_stop - time_start;
        double time = duration_cast<nanoseconds>(duration).count() / 1e9;

        miners_statistic.print("Time: " + std::to_string(time) + 's');

        miners_statistic.print("Done.\n");
    };

    while (true) {
        pool_client.init();
        bool _ = false;
        current_block = pool_client.get_new_block(_);

        mining();
    }
}

int main(int argc, char **argv) {
    run_miner(argc, argv);
    /*std::ifstream input("blocks_dataset.txt");
    std::vector<std::pair<fast_string, Statistic>> blocks;
    for (int i = 0; i < 651; i++) {
        auto [block_bytes_data, state] = read_block_data(input);
        //std::cout << reverse_str(bytes_to_hex(state.get_best().second.to_str())) << std::endl;
        blocks.push_back({block_bytes_data, state});
    }*/

    // >39 нулевых байт в хеше мы не получили

    // 27-й бит очень похож, чтобы был 1 в лучших nonce

    // 0, 1, 7, 8, 9, 10, 15, 17, 20, 22, 23, 24, 27, 29, 30

    /*std::vector<uint32_t> counts(32);
    uint32_t total = 0;
    for (auto [block_bytes_data, state]: blocks) {
        for (uint32_t index = 35; index < Statistic::LEN; index++) {
            if (state.counts[index] > 0) {
                total++;
                uint32_t nonce = state.exemplar[index];
                for (uint32_t bit = 0; bit < 32; bit++) {
                    counts[bit] += ((nonce >> bit) & 1);
                }
            }
        }
    }
    std::cout << total << '\n';
    std::vector<std::pair<double, uint32_t>> s;
    for (uint32_t bit = 0; bit < 32; bit++) {
        s.push_back({counts[bit] * 1.0 / total - 0.5, bit});
        //std::cout << bit << ' ' << counts[bit] * 1.0 / total - 0.5 << '\n';
    }
    std::sort(s.begin(), s.end());
    for (auto [p, c]: s) {
        std::cout << c << ' ' << p << '\n';
    }*/


    /*
bit (counts[bit] / all - 0.5)
28 -0.03149
31 -0.0299539
3 -0.0268817
25 -0.0192012
13 -0.0176651
2 -0.016129
11 -0.016129
18 -0.016129
26 -0.016129
12 -0.0130568
19 -0.0130568
4 -0.0115207
6 -0.00537634
21 -0.00384025
5 -0.00230415
16 -0.00230415
14 -0.000768049
10 0.00230415
29 0.00230415
17 0.00384025
23 0.00384025
22 0.00691244
7 0.00998464
24 0.0115207
20 0.0145929
9 0.0176651
8 0.0238095
30 0.0253456
1 0.0268817
15 0.0360983
0 0.0422427
27 0.077573
     */

    /*std::vector<uint32_t> counts(32);
    uint32_t total = 0;
    for (auto [block_bytes_data, state]: blocks) {
        if(state.get_best().second.builtin_ctz() >= 35) {
            total++;

            uint32_t nonce = state.get_best().first;
            for(uint32_t bit = 0; bit < 32; bit++){
                counts[bit] += ((nonce >> bit) & 1);
            }
        }
    }
    std::cout << total << '\n';
    std::vector<std::pair<double, uint32_t>> s;
    for(uint32_t bit = 0; bit < 32; bit++){
        s.push_back({counts[bit] * 1.0 / total - 0.5, bit});
        //std::cout << bit << ' ' << counts[bit] * 1.0 / total - 0.5 << '\n';
    }
    std::sort(s.begin(), s.end());
    for(auto [p, c] : s){
        std::cout << c << ' ' << p << '\n';
    }*/

    // сумма цифр/битов в лучших nonce
    /*uint32_t min_sum = 1e9;
    uint32_t max_sum = 0;
    uint32_t total_sum = 0;
    int count = 0;
    for (auto [block_bytes_data, state]: blocks) {
        if(state.get_best().second.builtin_ctz() <= 39) {
            count++;

            uint32_t nonce = state.get_best().first;
            uint32_t sum = 0;
            while (nonce > 0) {
                sum += nonce % 2;
                nonce /= 2;
            }
            total_sum += sum;
            if (sum < min_sum) {
                std::cout << "MIN: " << state.get_best().first << '\n';
                min_sum = sum;
            }

            if (sum > max_sum) {
                std::cout << "MAX: " << state.get_best().first << '\n';
                max_sum = sum;
            }
        }
    }
    std::cout << count << ' ' << min_sum << ' ' << max_sum << '\n';
    std::cout << total_sum * 1.0 / count << '\n';*/
}

// сумма цифр
/*MIN: 1787677545
MAX: 1787677545
MIN: 1674270048
MIN: 2307701059
MIN: 1002205154
MAX: 3863979357
MAX: 3592889498
MAX: 3978375789
MIN: 240320230
MAX: 2999889875
16 74*/

// сумма битов
/*
MIN: 1787677545
MAX: 1787677545
MIN: 1674270048
MIN: 3896513875
MAX: 1002205154
MAX: 3178610023
MAX: 2144215036
MAX: 3719559035
MIN: 141576349
MIN: 33888263
MAX: 266861567
8 25
*/