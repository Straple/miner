#include "miner.hpp"
#include "bits_manipulation.hpp"
#include "utils.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <utility>

using namespace std::chrono;

void Miner::Init(uint32_t ID, block new_block) {
    id = ID;
    b = std::move(new_block);
    b.build_extranonce2();
    logger = "worker" + std::to_string(ID) + ".txt";
    thread = std::thread(&Miner::run, this);
}

void Miner::set_new_block(block new_block) {
    auto time_start = steady_clock::now();

    mutex = mutex_t::WAITED_FOR_LOCK;
    while (mutex != mutex_t::LOCKED) {}

    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    logger.print("SET NEW BLOCK, waited time: " + std::to_string(duration_cast<nanoseconds>(duration).count() / 1e9) + "s");
    b = std::move(new_block);
    b.build_extranonce2();

    mutex = mutex_t::UNLOCKED;
}

block Miner::get_best_block() {
    auto time_start = steady_clock::now();

    mutex = mutex_t::WAITED_FOR_LOCK;
    while (mutex == mutex_t::WAITED_FOR_LOCK) {}

    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    logger.print("GET BEST BLOCK, waited time: " + std::to_string(duration_cast<nanoseconds>(duration).count() / 1e9) + "s");

    block copy_best_block = best_block;
    is_good = false;// теперь нам незачем этот гуд

    mutex = mutex_t::UNLOCKED;
    return copy_best_block;
}

bool Miner::available_good() const {
    return is_good;
}

// симулирует один раунд копания
// возвращает статистику хешей за этот раунд
// обновляет best_block
Statistic Miner::dig() {
    // counts[x] = количество вычисленных хешей с x нулями впереди
    std::vector<uint64_t> counts(257);

    auto register_hash = [&](std::string hash) {
        //hash[0] = 0;
        //hash[1] = 0b101;
        uint32_t x = 0;
        for (; x < 32 && hash[x] == 0; x++) {
        }
        int bit = 7;
        for (; bit >= 0 && ((hash[x] >> bit) & 1) == 0; bit--) {
        }
        counts[x * 8 + (7 - bit)]++;
        /*int kek = x * 8 + (7 - bit);
        std::cout << "<";
        for(int index = 0; index < hash.size(); index++){
            for(int bit = 7; bit >= 0; bit--){
                std::cout << ((hash[index] >> bit) & 1);
            }
        }
        std::cout << ">" << kek << "\n";
        exit(0);*/
    };

    std::mt19937 rnd(42);
    for(int kek = 0; kek < 10; kek++) {
        b.extranonce2 = integer_to_hex(rnd(), b.extranonce2_size * 2);
        b.build_data();

        for (int step = 0; step < 1'000; step++) {
            uint32_t nonce = rnd();
            std::string hash = b.calc_hash(nonce);
            //register_hash(hash);

            bool run = true;
            while (run) {
                run = false;
                for (uint32_t x: data) {
                    uint32_t new_nonce = nonce ^ x;
                    std::string new_hash = b.calc_hash(new_nonce);
                    //register_hash(new_hash);
                    if (new_hash < hash) {
                        nonce = new_nonce;
                        hash = new_hash;
                        run = true;
                    }
                }
            }

            register_hash(hash);

            if (hash < best_block_hash) {
                best_block_hash = hash;
                best_block = b;
                is_good = hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0 && hash[4] == 0;
                logger.print("NEW BEST BLOCK: " + bytes_to_hex(best_block_hash));
            }
        }
    }

    return counts;
}

void Miner::simulate_lock() {
    if (mutex == mutex_t::WAITED_FOR_LOCK) {
        mutex = mutex_t::LOCKED;
        while (mutex == mutex_t::LOCKED) {}
    }
}

[[noreturn]] void Miner::run() {
    while (true) {
        simulate_lock();

        // 0.956058 0.0437581 0.000183754
        auto statistic = dig();
        /*std::stringstream ss;
        ss << "STATISTIC: " << statistic;
        logger.print(ss.str());*/

        std::cout << statistic.calc() << "->";
        std::cout.flush();

        /*double temperature = 1;
        int k = 1'000;
        for(int i = 0; i < k; i++){
            temperature *= 0.99;

            uint32_t index = get_random_engine()() % 32;
            uint32_t x = (1ULL << (get_random_engine()() % 32));
            data[index] ^= x;
            auto new_statistic = dig();

            double P = double(get_random_engine()()) / UINT32_MAX;
            double Q = exp((new_statistic.calc() - statistic.calc()) / temperature);
            std::cout << i << ' ' << temperature << ' ' << P << ' ' << Q << ' ' << (new_statistic.calc() - statistic.calc()) << '\n';
            if (new_statistic.calc() > statistic.calc() || P < Q) {
                statistic = new_statistic;
                //std::cout << statistic.calc() << "->";
                //std::cout.flush();

                std::stringstream ss;
                ss << "STATISTIC: " << statistic << " [";
                for(auto val : data){
                    ss << val << ", ";
                }
                ss << ']';
                logger.print(ss.str());
            } else {
                data[index] ^= x;
            }
        }*/

        // 0.00000 0.00000 0.00200 0.04600 0.14400 0.23900 0.21300 0.14900 0.08100 0.05700 0.01800 0.01300 0.00800 0.00400 0.00300 0.00300 0.00400 0.00200 0.00400 0.00500 0.00100 0.00100 0.00200 0.00000 0.00000 0.00000 0.00000 0.00100
        // 0.00000 0.00000 0.00400 0.04100 0.14100 0.23400 0.24400 0.13900 0.07900 0.05400 0.01500 0.01600 0.00700 0.00500 0.00100 0.00400 0.00200 0.00300 0.00100 0.00300 0.00100 0.00200 0.00200 0.00000 0.00200

        while (true) {

            for(int index = 0; index < data.size(); index++){
                for(int bit = 0; bit < 32; bit++){
                    uint32_t x = (1ULL << bit);
                    data[index] ^= x;

                    auto new_statistic = dig();

                    if (new_statistic < statistic) {
                        statistic = new_statistic;

                        std::cout << statistic.calc() << "->";
                        std::cout.flush();

                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(5);
                        ss << "STATISTIC: " << statistic << " [";

                        for (auto val: data) {
                            ss << val << ", ";
                        }
                        ss << ']';
                        logger.print(ss.str());
                    }
                    else{
                        data[index] ^= x;
                    }
                }
            }
        }
        std::cout << std::endl;
        exit(0);

        /*for (uint32_t step = 0; step < 10; step++) {
                uint32_t index = get_random_engine()() % 32;
                uint32_t x = (1ULL << (get_random_engine()() % 32));
                data[index] ^= x;
                auto new_statistic = dig();

        if (new_statistic < best_statistic) {
            best_statistic = new_statistic;
            best_data = data;
            /*statistic = new_statistic;
                    std::cout << "random: " << statistic.calc() << "->";
                    std::cout.flush();

                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(5);
                    ss << "STATISTIC: " << statistic << " [";

                    for (auto val: data) {
                        ss << val << ", ";
                    }
                    ss << ']';
                    logger.print(ss.str());*/
            /*} else {

                }
                data[index] ^= x;
            }*/

            //if (!run) {
            /* std::cout << "try shuffle->";
                for (int k = 0; k < 1; k++) {
                    auto old_data = data;
                    for (auto &x: data) {
                        x = get_random_engine()();
                    }
                    auto new_statistic = dig();
                    if (new_statistic < statistic) {
                        statistic = new_statistic;
                        std::cout << "shuffle: " << statistic.calc() << "->";
                        std::cout.flush();

                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(5);
                        ss << "STATISTIC: " << statistic << " [";

                        for (auto val: data) {
                            ss << val << ", ";
                        }
                        ss << ']';
                        logger.print(ss.str());

                        run = true;
                    } else {
                        data = old_data;
                    }
                }
            //}*/
    }
    /*auto time_start = steady_clock::now();
    for (uint32_t nonce = 1; true; nonce++) {

        if (nonce % 0x1000 == 0 && mutex == mutex_t::WAITED_FOR_LOCK) {
            mutex = mutex_t::LOCKED;
            while (mutex == mutex_t::LOCKED) {}
        }

        std::string hash = b.calc_hash(nonce);
        //ASSERT(b.calc_hash(nonce) == b.trivial_calc_hash(nonce), "failed");

        if (hash < best_block_hash) {
            best_block_hash = hash;
            best_block = b;
            is_good = hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0 && hash[4] == 0;
            logger.print("NEW BEST BLOCK: " + bytes_to_hex(best_block_hash));
        }

        if (nonce % 40'000'000 == 0) {
            auto time_stop = steady_clock::now();
            auto duration = time_stop - time_start;
            time_start = time_stop;

            int64_t hashrate =
                    1.0 * 40'000'000 / (duration_cast<nanoseconds>(duration).count() / 1e9);
            logger.print("HASHRATE: " + pretty_hashrate(hashrate));

            b.build_extranonce2();// продолжим с новым значением extranonce2
        }
    }*/
}
