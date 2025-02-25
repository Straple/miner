#pragma once

// Miner занимается вычислением хешей блоков и аккумулирует самый лучший (с меньшим хешом)

#include <atomic>
#include <thread>

#include "block.hpp"
#include "logger.hpp"

// синхронизация майнера и главного потока
/*
// поток B работает при mutex_t::UNLOCKED
// это значит, что никто другой его данные не трогает

// есть поток A, который периодически хочет что-то сделать с данными потока B
// тогда он дает сигнал, что хочет этого: mutex_t::WAITED_FOR_LOCK
// а поток B меняет его на LOCKED и не работает при нем

// поток A
// говорим потоку B, что хотим взять мютекс
mutex = mutex_t::WAITED_FOR_LOCK;
// ждем когда он заметит это и даст нам блокировку
while (mutex == mutex_t::WAITED_FOR_LOCK) {}
// ... делаем свои дела под мютексом
mutex = mutex_t::UNLOCKED; // закончили

// поток B
if (mutex == mutex_t::WAITED_FOR_LOCK) {
    // поток A ожидает взятия мютекса
    mutex = mutex_t::LOCKED; // даем ему эту возможность
    // и теперь ожидаем когда A снимет блокировку
    while (mutex != mutex_t::UNLOCKED) {} // <--- и вот тут прикол
    // поток A говорит
    // mutex = mutex_t::UNLOCKED
    // а затем сразу же
    // mutex = mutex_t::WAITED_FOR_LOCK
    // и тогда поток B будет вечно крутиться, ожидая когда он mutex станет UNLOCKED
    // правильно будет
    while(mutex == mutex_t::LOCKED){}
}
*/

#include "statistic.hpp"
#include "utils.hpp"

class Miner {
    std::thread thread;

    lite_block current_block;// текущий блок, который мы добываем

    uint32_t best_nonce;
    bool is_good = false;

    Statistic state;

    // Logger logger;

    int64_t hash_calculated_count = 0;

    double work_time = 0;

    // закончена ли работа
    bool done = false;

public:
    void init(lite_block new_block, std::vector<std::atomic<bool>> &visited);

    uint32_t get_best_nonce();

    [[nodiscard]] int64_t get_hashrate() const;

    Statistic &get_statistic();

    [[nodiscard]] bool available_good() const;

    [[nodiscard]] bool is_done() const;

    void join();

    void run(std::vector<std::atomic<bool>> &visited);
};
