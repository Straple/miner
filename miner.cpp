#include "miner.hpp"
#include "bits_manipulation.hpp"
#include "utils.hpp"
#include <chrono>
#include <iostream>

using namespace std::chrono;

void Miner::set_new_block(block new_block) {
    auto time_start = steady_clock::now();
    std::unique_lock locker{mutex};
    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    std::cout << "SET NEW BLOCK FOR THREAD " << id << " WAITED: " << (duration_cast<nanoseconds>(duration).count() / 1e9) << "s" << std::endl;
    b = new_block;
}

block Miner::get_best_block() {
    auto time_start = steady_clock::now();
    std::unique_lock locker{mutex};
    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    std::cout << "GET BEST BLOCK " << id << " WAITED: " << (duration_cast<nanoseconds>(duration).count() / 1e9) << "s" << std::endl;
    return best_block;
}

[[noreturn]] void Miner::run() {
    auto time_start = steady_clock::now();
    mutex.lock();// не нужно сейчас трогать наши данные другим потоком
    for (uint32_t nonce = 0; true; nonce++) {
        std::string hash = b.calc_hash(nonce);
        //ASSERT(b.calc_hash(nonce) == b.trivial_calc_hash(nonce), "failed");

        if (hash < best_block_hash) {
            best_block_hash = hash;
            best_block = b;
            std::cout << "NEW BEST BLOCK <" << id << "> " << bytes_to_hex(best_block_hash) << std::endl;
        }

        if (nonce % 40'000'000 == 0) {
            mutex.unlock();// даем возможность что-то у нас изменить другому потоку
            auto time_stop = steady_clock::now();
            auto duration = time_stop - time_start;
            time_start = time_stop;

            int64_t hashrate =
                    1.0 * 40'000'000 / (duration_cast<nanoseconds>(duration).count() / 1e9);
            std::cout << "Worker " << id << " " << pretty_hashrate(hashrate) << std::endl;

            std::this_thread::sleep_for(nanoseconds(1000));
            mutex.lock();// в наших мозгах поковырялись, теперь можем продолжить майнить

            b.build_extranonce2();
        }
    }
}
