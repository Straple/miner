#pragma once

#include "fast_string.hpp"
#include "lite_block.hpp"
#include "statistic.hpp"
#include "utils.hpp"
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

std::pair<uint32_t, uint32_t> search_block(fast_string block_bytes_data, Statistic state) {
    std::mt19937 gen(42);
    std::uniform_int_distribution<uint32_t> distrib(0, UINT32_MAX);
    auto rnd = [&]() {
        return distrib(gen);
    };

    lite_block cur_block(block_bytes_data);

    auto f = [&](uint32_t x) {
        return cur_block.calc_hash(x).builtin_ctz();
    };

    uint32_t best_x = 0;
    uint32_t best_f = 0;

    auto time_start = steady_clock::now();

    //#define PRINT_UPDATE_CUR
    //#define PRINT_UPDATE_BEST

    for (int run_number = 0; true; run_number++) {

        if(run_number % 100 == 0) {
            auto time_stop = steady_clock::now();
            auto duration = time_stop - time_start;
            double time = duration_cast<nanoseconds>(duration).count() / 1e9;

            if (time > 30) {
                break;
            }
        }

        uint32_t x = rnd();
        uint32_t f_x = f(x);

#ifdef PRINT_UPDATE_CUR
        std::cout << f(x) << "->";
#endif

        //int steps = 1e8;
        /*while (true) {
            uint32_t to_x = x;
            uint32_t to_f_x = f_x;
            for(int a = 0; a < 32; a++){
                uint32_t new_x = x ^ (1ULL << a);
                uint32_t new_f_x = f(new_x);
                if (to_f_x < new_f_x) {
                    to_x = new_x;
                    to_f_x = new_f_x;
                }
            }
            for(int a = 0; a < 32; a++){
                for(int b = a + 1; b < 32; b++) {
                    uint32_t new_x = x ^ (1ULL << a) ^ (1ULL << b);
                    uint32_t new_f_x = f(new_x);
                    if (to_f_x < new_f_x) {
                        to_x = new_x;
                        to_f_x = new_f_x;
                    }
                }
            }
            if(to_x == x){
                break;
            }
            x = to_x;
            f_x = to_f_x;
        }*/
#ifdef PRINT_UPDATE_CUR
        std::cout << std::endl;
#endif

        if (best_f < f_x) {
            best_f = f_x;
            best_x = x;
#ifdef PRINT_UPDATE_BEST
            std::cout << best_f << "->";
#endif

        }
    }
#ifdef PRINT_UPDATE_BEST
    std::cout << std::endl;
#endif
    return {best_x, best_f};
}

void super_search(std::vector<std::pair<fast_string, Statistic>> blocks) {
    // lite_block b(blocks[0].first);
    // std::cout << bytes_to_hex(reverse_str(b.calc_hash(blocks[0].second.get_best().first))) << std::endl;

    //#define PRINT_BLOCKS

    blocks.resize(96);

    const int N = blocks.size();

    // запускаем воркеров
    std::vector<std::atomic<bool>> visited(N);

    uint64_t accum = 0;

    std::mutex mutex;

    auto do_work = [&](int block_id, const fast_string &block_bytes_data, const Statistic &state) {
        auto time_start = steady_clock::now();

#ifdef PRINT_BLOCKS
        std::cout << "block: " << i << std::endl;
#endif
        auto [best_x, best_f] = search_block(block_bytes_data, state);
#ifdef PRINT_BLOCKS
        std::cout << best_x << ' ' << best_f << '\n'
                  << bytes_to_hex(reverse_str(lite_block(blocks[i].first).calc_hash(best_x))) << std::endl;
#endif

        auto time_stop = steady_clock::now();
        auto duration = time_stop - time_start;
        double time = duration_cast<nanoseconds>(duration).count() / 1e9;

        int64_t c = std::abs(static_cast<int>(best_f) - static_cast<int>(state.get_best().second.builtin_ctz()));

        mutex.lock();
        std::cout << block_id << ' ' << state.get_best().second.builtin_ctz() - best_f << ' ' << time << 's' << ' ' << accum << std::endl;
        accum += c * c;
        mutex.unlock();
    };

    auto run_worker = [&]() {
        for (int i = 0; i < N; i++) {
            bool expected = false;
            if (visited[i].compare_exchange_strong(expected, true)) {
                do_work(i, blocks[i].first, blocks[i].second);
            }
        }
    };

    std::vector<std::thread> thrs(12);
    for (int thread_id = 0; thread_id < 12; thread_id++) {
        thrs[thread_id] = std::thread(run_worker);
    }

    for (int thread_id = 0; thread_id < 12; thread_id++) {
        thrs[thread_id].join();
    }

    std::cout << accum * 1.0 / N << std::endl;
}

// 168.41 -> 59.48 -> 55.59 -> 54.43 -> 46.41 -> 43.6875 -> 42.3854 -> 39.89

//9 2 30.0083s
//4 5 30.0189s
//11 7 30.0239s
//10 7 30.0428s
//6 15 30.0516s
//3 7 30.0567s
//8 7 30.0693s
//5 8 30.0716s
//0 15 30.1439s
//7 8 30.1477s
//1 8 30.2277s
//2 4 30.2628s
//12 6 30.0367s
//15 6 30.0725s
//16 9 30.0739s
//18 7 30.0597s
//14 8 30.1217s
//13 6 30.1586s
//20 9 30.0345s
//17 10 30.247s
//19 11 30.2331s
//21 11 30.2518s
//22 9 30.1804s
//23 9 30.1593s
//30 7 30.0178s
//25 6 30.101s
//28 9 30.0762s
//26 9 30.1459s
//24 8 30.25s
//27 3 30.2004s
//32 9 30.0445s
//31 10 30.0886s
//29 5 30.2784s
//34 7 30.0561s
//35 12 30.0589s
//33 6 30.1779s
//38 4 30.1043s
//37 8 30.1348s
//36 6 30.2074s
//39 1 30.1503s
//40 9 30.1753s
//41 6 30.1534s
//42 6 30.1931s
//43 3 30.1892s
//45 9 30.1453s
//44 8 30.1663s
//46 5 30.1645s
//47 6 30.2159s
//48 9 30.0635s
//51 6 30.0042s
//54 5 30.0133s
//49 7 30.2342s
//55 3 30.0233s
//57 7 30.0099s
//58 10 30.0149s
//50 3 30.2697s
//52 4 30.2298s
//53 7 30.256s
//56 4 30.242s
//59 7 30.277s
//60 9 30.1257s
//61 9 30.16s
//62 7 30.1988s
//63 6 30.1693s
//65 8 30.1774s
//69 3 30.0889s
//64 0 30.245s
//68 2 30.151s
//66 9 30.1964s
//67 6 30.2499s
//70 5 30.2258s
//71 7 30.2694s
//72 1 30.1299s
//73 8 30.1336s
//74 1 30.128s
//77 7 30.0647s
//75 3 30.1869s
//76 8 30.1336s
//80 7 30.1063s
//79 11 30.1504s
//78 6 30.187s
//81 9 30.1455s
//82 10 30.1293s
//83 6 30.2077s
//84 9 30.0762s
//85 7 30.2148s
//86 8 30.223s
//87 6 30.2273s
//89 9 30.1873s
//90 6 30.1771s
//88 7 30.2349s
//91 9 30.1945s
//92 8 30.1844s
//93 3 30.1858s
//94 9 30.1464s
//95 5 30.1469s
//96 5 30.0208s
//97 3 30.1301s
//99 4 30.0684s
//98 5 30.0992s
//6.79
//
//Process finished with exit code 0


// 9.17

/*
9 9 7.05761s
6 11 7.07431s
10 7 7.07627s
7 10 7.08751s
1 11 7.10613s
5 8 7.11229s
2 11 7.11437s
3 11 7.11484s
0 14 7.11998s
8 10 7.12213s
4 10 7.12942s
11 11 7.17603s
12 12 6.95917s
16 15 6.92917s
13 9 6.99451s
17 7 6.96105s
19 13 6.96529s
15 8 7.00489s
22 8 6.96766s
21 11 6.98497s
18 10 6.99841s
14 5 7.03915s
20 11 7.00142s
23 10 6.99815s
26 10 7.01493s
25 8 7.05806s
33 11 7.03008s
32 11 7.04739s
27 7 7.0961s
24 9 7.15836s
30 11 7.07972s
31 13 7.1128s
28 11 7.15981s
29 6 7.16285s
34 13 7.14393s
35 11 7.12571s
37 5 7.06073s
36 7 7.08197s
41 10 7.00442s
38 9 7.06906s
40 9 7.04597s
39 8 7.09159s
42 8 7.09681s
43 5 7.06595s
45 7 7.04721s
44 11 7.10212s
46 8 7.1082s
47 9 7.08934s
52 7 6.94158s
49 5 7.00457s
50 4 7.0159s
48 5 7.05243s
51 9 7.00092s
56 9 6.9643s
53 11 7.01658s
55 8 6.99006s
54 5 7.02278s
57 9 6.98017s
58 11 7.00728s
59 10 7.05239s
60 10 7.07368s
62 10 7.04755s
63 7 7.06574s
61 13 7.10904s
64 14 7.10133s
66 7 7.0563s
67 8 7.10349s
65 5 7.14352s
69 11 7.10997s
68 6 7.14288s
70 8 7.11386s
71 8 7.0829s
72 7 7.06316s
73 11 7.08529s
74 8 7.0679s
75 9 7.07145s
76 10 7.05338s
77 5 7.05332s
79 13 7.0342s
80 9 7.0194s
78 7 7.09594s
81 10 7.03799s
83 8 7.03059s
82 11 7.06485s
85 10 7.06297s
89 9 7.02494s
87 8 7.05277s
84 9 7.11787s
86 9 7.08443s
88 8 7.09614s
91 11 7.01988s
95 12 6.99163s
90 4 7.11611s
93 11 7.10313s
92 12 7.13691s
94 10 7.10026s
97 11 3.90844s
96 11 3.9249s
98 8 3.95143s
99 7 3.98023s
9.17

Process finished with exit code 0
*/