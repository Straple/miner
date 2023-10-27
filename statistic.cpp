#include "statistic.hpp"

#include "bits_manipulation.hpp"

bool Statistic::add(uint32_t nonce, const fast_string &hash) {
    uint32_t index = hash.builtin_ctz();
    bool result = best_index < index || (best_index == index && hash < exemplar_hash[index]);

    best_index = std::max(best_index, index);
    if (counts[index] == 0 || hash < exemplar_hash[index]) {
        exemplar[index] = nonce;
        exemplar_hash[index] = hash;
    }
    counts[index]++;
    return result;
}

void Statistic::add(const Statistic &other) {
    for (uint32_t index = 0; index < LEN; index++) {
        if (other.counts[index] != 0) {
            best_index = std::max(best_index, index);

            if (counts[index] == 0 || other.exemplar_hash[index] < exemplar_hash[index]) {
                exemplar[index] = other.exemplar[index];
                exemplar_hash[index] = other.exemplar_hash[index];
            }
            counts[index] += other.counts[index];
        }
    }
}

std::pair<uint32_t, fast_string> Statistic::get_best() const {
    ASSERT(best_index < LEN && counts[best_index] != 0, "bad best_index");
    return {exemplar[best_index], exemplar_hash[best_index]};
}

std::ostream &operator<<(std::ostream &output, const Statistic &state) {
    output << "COUNTS: ";
    for (auto &item: state.counts) {
        output << item << ' ';
    }
    output << "\nEXEMPLARS: ";
    for (auto &item: state.exemplar) {
        output << item << ' ';
    }
    output << "\nEXEMPLARS_HASH: ";
    for (auto &item: state.exemplar_hash) {
        output << bytes_to_hex(reverse_str(item)) << ' ';
    }
    return output;
}