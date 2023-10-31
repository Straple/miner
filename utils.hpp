#pragma once

#include <random>

std::mt19937 &get_random_engine();

uint32_t rnd();

// [left, right]
int rnd(int left, int right);

// [0, 1]
double rnd_chance();

std::string pretty_hashrate(uint64_t hashrate);