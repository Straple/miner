#pragma once

#include <cmath>
#include <iostream>
#include <vector>

class Statistic {
    std::vector<double> data;

public:
    Statistic(const std::vector<uint64_t> &counts);

    friend std::ostream &operator<<(std::ostream &output, const Statistic &s);

    // lhs лучше rhs
    // lhs лучше, если в нем гораздо больше хешей с большим количеством нулей
    friend bool operator<(const Statistic &lhs, const Statistic &rhs);

    // возвращает некоторое число, чем оно больше, тем эта статистика лучше,
    // то есть в ней много хешей с большим количеством нулей
    double calc() const;
};