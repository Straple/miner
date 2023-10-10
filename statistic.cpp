#include "statistic.hpp"

Statistic::Statistic(const std::vector<uint64_t> &counts) {
    uint64_t total_count = 0;
    for (uint32_t x = 0; x <= 256; x++) {
        total_count += counts[x];
    }

    data.resize(257);
    for (uint32_t x = 0; x <= 256; x++) {
        data[x] = static_cast<double>(counts[x]) / total_count;
    }
}

std::ostream &operator<<(std::ostream &output, const Statistic &s) {
    for (auto val: s.data) {
        output << val << ' ';
    }
    return output;
}

double Statistic::calc() const {
    double result = 0;
    //result += 1 - data[0];
    double ost = 1;
    for (int index = 0; index <= 256; index++) {
        result += data[index] * pow(sqrt(2), index + 1);
        ost -= data[index];
        //result += index * std::pow(1 + data[index], index*index + 1);
    }
    return result;
}

bool operator<(const Statistic &lhs, const Statistic &rhs) {
    return lhs.calc() > rhs.calc();
    /*for(int index = 32; index >= 0; index--){
        if(lhs.data[index] )
    }
    return false;*/
}