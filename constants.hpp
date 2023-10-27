#pragma once

#include <string>

const uint32_t FAST_STRING_LEN = 80;

constexpr static uint32_t MAIN_THREAD_SLEEP_TIME_MS = 10;

constexpr uint64_t NONCE_BOUND = (1ULL << 24);
//constexpr uint32_t TASK_BLOCKS_COUNT = 1024 * 8;
//constexpr uint32_t TASK_BLOCK_LEN = NONCE_BOUND / TASK_BLOCKS_COUNT;

// solo.ckpool.org
// bs.poolbinance.com
// pool.veriblock.cc
const static std::string IP_ADDRESS = "btc.poolbinance.com";
const static std::string PORT = "1800";                       //"3333";
const static std::string WORKER_NAME = "MyAlgoMiner19937.001";//"16p9y6EstGYcnofGNvUJMEGKiAWhAr1uR8";
