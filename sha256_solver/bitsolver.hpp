#pragma once

#include <iostream>

#include <bitset>

struct bitsolver {
    // x = количество неизвестных
    // 2^x = количество возможных значений переменных
    // 2^2^x = количество возможных формул
    // x=8
    // 256 битов нужно, чтобы закодировать
    std::bitset<256> msk;

    bitsolver() = default;

    // конструктор от скаляра 0 или 1
    explicit bitsolver(bool value) {
        if (value) {
            msk = ~msk;
        }
    }

    // конструктор от номера неизвестной переменной
    explicit bitsolver(int x) {
        for (uint64_t bit = 0; bit < (1 << 8); bit++) {
            msk[bit] = (((bit >> x) & 1) != 0);
        }
    }

    // x y z res
    // 0 0 0  0
    // 0 0 1  0
    // 0 1 0  1
    // 0 1 1  1
    // 1 0 0  0
    // 1 0 1  0
    // 1 1 0  1
    // 1 1 1  1
};

inline std::ostream &operator<<(std::ostream &output, const bitsolver &bs) {
    return output << bs.msk;
}

inline bitsolver operator!(bitsolver bs) {
    bs.msk = ~bs.msk;
    return bs;
}

inline bitsolver operator&(bitsolver lhs, bitsolver rhs) {
    lhs.msk &= rhs.msk;
    return lhs;
}

inline bitsolver operator|(bitsolver lhs, bitsolver rhs) {
    lhs.msk |= rhs.msk;
    return lhs;
}

inline bitsolver operator^(bitsolver lhs, bitsolver rhs) {
    lhs.msk ^= rhs.msk;
    return lhs;
}

inline bool operator==(const bitsolver lhs, const bitsolver rhs) {
    return lhs.msk == rhs.msk;
}

inline bool operator!=(const bitsolver lhs, const bitsolver rhs) {
    return !(lhs == rhs);
}