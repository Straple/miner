#pragma once

// Miner занимается вычислением хешей блоков и аккумулирует самый лучший (с меньшим хешом)

#include <mutex>
#include <thread>

#include "block.hpp"

class Miner {
    std::thread thread;
    std::mutex mutex;

    uint32_t id = -1;// id майнера

    block b;// текущий блок, который мы добываем

    std::string best_block_hash = "z";
    block best_block;

public:
    Miner() = default;

    void Init(uint32_t ID, block new_block) {
        id = ID;
        b = std::move(new_block);
        thread = std::thread(&Miner::run, this);
    }

    // ставит новый блок на добычу
    void set_new_block(block new_block);

    // дает лучший блок
    block get_best_block();

    [[noreturn]] void run();
};
