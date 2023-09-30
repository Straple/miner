#pragma once

#include <random>

std::mt19937 &get_random_engine();

uint32_t rnd();

std::string pretty_hashrate(uint64_t hashrate);