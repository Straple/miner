#pragma once

#include <iostream>

template <typename T>
struct solver {
    T val = 0;

    // X - тот байт, который мы хотим найти

    // является ли он X? или промежуточным значением X
    // если true, то он нам интересен
    bool is_X = false;

    // dp[X] = какое значение мы получим, если подставить такое X
    T dp[256];

    void clear_dp() {
        for (uint32_t x = 0; x < 256; x++) {
            dp[x] = x;
        }
    }

    void set_is_X() {
        is_X = true;
        clear_dp();
    }

    solver(T val_) : val(val_) {
    }

    template <typename R>
    solver(solver<R> other) : val(other.val), is_X(other.is_X) {
        for (uint32_t x = 0; x < 256; x++) {
            dp[x] = other.dp[x];
        }
    }

    solver() = default;

    solver operator~() const {
        solver result = *this;
        result.val = ~result.val;
        for (uint32_t x = 0; x < 256; x++) {
            result.dp[x] = ~dp[x];
        }
        return result;
    }

    solver operator>>(uint64_t k) const {
        solver result = *this;
        result.val >>= k;
        for (uint32_t x = 0; x < 256; x++) {
            result.dp[x] = dp[x] >> k;
        }
        return result;
    }

    solver operator<<(uint64_t k) const {
        solver result = *this;
        result.val <<= k;
        for (uint32_t x = 0; x < 256; x++) {
            result.dp[x] = dp[x] << k;
        }
        return result;
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &output, const solver<T> &s) {
    output << "solutions: {\n";
    for (int x = 0; x < 256; x++) {
        output << "(" << x << "->" << static_cast<uint64_t>(s.dp[x]) << ")\n";
    }
    return output << "}";
}

#define ADD_OOP(oop)                                              \
    template <typename T>                                         \
    solver<T> operator oop(solver<T> lhs, const solver<T> &rhs) { \
        if (lhs.is_X) {                                           \
            if (rhs.is_X) {                                       \
                lhs.val = lhs.val oop rhs.val;                    \
                for (uint32_t x = 0; x < 256; x++) {              \
                    lhs.dp[x] = lhs.dp[x] oop rhs.dp[x];          \
                }                                                 \
                return lhs;                                       \
            } else {                                              \
                lhs.val = lhs.val oop rhs.val;                    \
                for (uint32_t x = 0; x < 256; x++) {              \
                    lhs.dp[x] = lhs.dp[x] oop rhs.val;            \
                }                                                 \
                return lhs;                                       \
            }                                                     \
        } else if (rhs.is_X) {                                    \
            return rhs oop lhs;                                   \
        } else {                                                  \
            return lhs.val oop rhs.val;                           \
        }                                                         \
    }

ADD_OOP(+)
ADD_OOP(-)
ADD_OOP(*)
ADD_OOP(&)
ADD_OOP(^)
ADD_OOP(|)
