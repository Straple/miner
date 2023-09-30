#pragma once

// Miner занимается вычислением хешей блоков и аккумулирует самый лучший (с меньшим хешом)

#include <mutex>
#include <thread>

#include "block.hpp"

class Miner {
    std::thread thread;
    std::mutex mutex;

    uint32_t id; // id майнера

    block b; // текущий блок, который мы добываем

    std::string best_block_hash = "z";
    block best_block;
    bool best_block_is_submited = false;

    bool is_new_block = false;
    block new_block;

    // берет новый блок из best_block, если такой есть
    void keep_new_block();

    // best_block = b
    // best_block_hash = hash
    void set_best_block(const std::string &hash);

public:

    // ставит новый блок
    void set_new_block(block new_block_);

    // дает лучший блок
    block get_best_block();

    void run();
};
