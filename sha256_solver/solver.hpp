#pragma once

template<typename T>
struct solver {


    solver() = default;

    solver(uint64_t val) {
        for (uint32_t bit = 0; bit < bits; bit++, val >>= 1) {
            data[bit] = bitsolver((val & 1) != 0);
        }
    }

    template<uint32_t other_bits>
    solver(const solver<other_bits> &other) {
        for (uint32_t bit = 0; bit < std::min(other_bits, bits); bit++) {
            data[bit] = other[bit];
        }
    }

    const bitsolver &operator[](uint32_t index) const {
        return data[index];
    }

    bitsolver &operator[](uint32_t index) {
        return data[index];
    }

    uint64_t calc(int x) {
        uint64_t res = 0;
        for (int bit = 0; bit < bits; bit++) {
            res |= uint64_t(data[bit].msk[x]) << bit;
        }
        return res;
    }
};

template<uint32_t bits>
std::ostream &operator<<(std::ostream &output, const solver<bits> &s) {
    std::cout << "solver<" << bits << ">:{\n";
    for (uint32_t bit = 0; bit < bits; bit++) {
        output << bit << ": " << s[bit] << ",\n";
    }
    std::cout << "}\n";
    return output;
}

template<uint32_t bits>
solver<bits> operator<<(solver<bits> s, uint32_t k) {
    solver<bits> res;
    for (uint32_t bit = 0; bit + k < bits; bit++) {
        res[bit + k] = s[bit];
    }
    return res;
}

template<uint32_t bits>
solver<bits> operator>>(solver<bits> s, uint32_t k) {
    solver<bits> res;
    for (uint32_t bit = k; bit < bits; bit++) {
        res[bit - k] = s[bit];
    }
    return res;
}

template<uint32_t bits>
solver<bits> operator&(solver<bits> lhs, const solver<bits> &rhs) {
    for (uint32_t bit = 0; bit < bits; bit++) {
        lhs[bit] = lhs[bit] & rhs[bit];
    }
    return lhs;
}

template<uint32_t bits>
solver<bits> operator|(solver<bits> lhs, const solver<bits> &rhs) {
    for (uint32_t bit = 0; bit < bits; bit++) {
        lhs[bit] = lhs[bit] | rhs[bit];
    }
    return lhs;
}

template<uint32_t bits>
solver<bits> operator^(solver<bits> lhs, const solver<bits> &rhs) {
    for (uint32_t bit = 0; bit < bits; bit++) {
        lhs[bit] = lhs[bit] ^ rhs[bit];
    }
    return lhs;
}

template<uint32_t bits>
solver<bits> operator~(solver<bits> s) {
    for (uint32_t bit = 0; bit < bits; bit++) {
        s[bit] = !s[bit];
    }
    return s;
}

template<uint32_t bits>
inline bool operator==(const solver<bits> lhs, const solver<bits> rhs) {
    for (uint32_t bit = 0; bit < bits; bit++) {
        if(lhs[bit] != rhs[bit]){
            return false;
        }
    }
    return true;
}

template<uint32_t bits>
inline bool operator!=(const solver<bits> lhs, const solver<bits> rhs) {
    return !(lhs == rhs);
}

template<uint32_t bits>
solver<bits> operator+(solver<bits> lhs, const solver<bits> &rhs) {
    //std::cout << "lhs: " << lhs << "\n";
    //std::cout << "rhs: " << rhs << "\n";
    //std::cout << "lhs & rhs: " << (lhs & rhs) << std::endl;

    bool need_add = false;
    for (uint32_t bit = 0; bit < bits; bit++) {
        need_add |= (rhs[bit] != bitsolver(false));
    }
    if (!need_add) {
        return lhs;
    } else {
        return (lhs ^ rhs) + ((lhs & rhs) << 1);
    }
}