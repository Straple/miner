#pragma once

// Miner занимается вычислением хешей блоков и аккумулирует самый лучший (с меньшим хешом)

#include <atomic>
#include <thread>

#include "block.hpp"
#include "logger.hpp"
#include "statistic.hpp"

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

#include "utils.hpp"

inline std::vector<uint32_t> default_data(){
    std::vector<uint32_t> data(100);
    for(uint32_t index = 0; index < data.size(); index++){
        data[index] = rnd();
    }
    return data;
}

class Miner {
    std::thread thread;

    enum class mutex_t {
        LOCKED,
        WAITED_FOR_LOCK,
        UNLOCKED,
    };
    std::atomic<mutex_t> mutex = mutex_t::LOCKED;

    uint32_t id = -1;// id майнера

    // эту штуку мы и будем обучать
    // это возможные шаги, которые мы делаем в dig() при град спуске
    std::vector<uint32_t> data = default_data();

    block b;// текущий блок, который мы добываем

    std::string best_block_hash = "z";
    block best_block;
    bool is_good = false;

    Logger logger;

    Statistic dig();

    void simulate_lock();

public:
    void Init(uint32_t ID, block new_block);

    // ставит новый блок на добычу
    void set_new_block(block new_block);

    // дает лучший блок
    block get_best_block();

    bool available_good() const;

    [[noreturn]] void run();
};
