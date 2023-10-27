#pragma once

// PoolClient взаимодействует с пулом:
// подключение, подписка, авторизация, получение новых задач, отправка решений

#include "block.hpp"
#include "logger.hpp"
#include <string>

#include <boost/asio.hpp>
using namespace boost::asio;
using ip::tcp;

class PoolClient {

    // ======================

    boost::asio::io_context io_context;
    tcp::iostream sockstream;

    // ======================

    std::string extranonce1;
    int extranonce2_size = 0;
    fast_string job_id;

    // ======================

    Logger logger;

    // connect + subscribe + authorize
    void init();

    // подписывается
    void subscribe();

    // авторизуется
    void authorize();

public:
    // подключается к пулу, подписывается и авторизуется
    PoolClient();

    // Берет новый блок у пула.
    // Пул может прислать какие-то другие блоки, которые тоже можно майнить
    // (там просто другие значения coinb1, extranonce1 и возможно прочего)
    // Но мы не очень хотим их брать, потому что и так по горло в задачах,
    // поэтому если new_miner_task = true, то это значит, что
    // майнинг старых блоков закончился и нужно брать другие задачи
    block get_new_block(bool &new_miner_task);

    // отправляет этот блок пулу
    void submit(const block &b);

    // вернет true, если можно что-то считать у пула
    bool reading_is_available();

    void update_connection();
};
