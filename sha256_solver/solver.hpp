#pragma once

#include <iostream>

inline constexpr static uint32_t N = 256;

template<typename T>
struct solver {
    // X - тот байт, который мы хотим найти

    // является ли он X? или промежуточным значением X
    // если true, то он нам интересен
    bool is_X = false;

    // dp[X] = какое значение мы получим, если подставить такое X
    T dp[N];

    void set_is_X() {
        is_X = true;
        for (uint32_t x = 0; x < N; x++) {
            dp[x] = x;
        }
    }

    solver(T val) {
        for (uint32_t x = 0; x < N; x++) {
            dp[x] = val;
        }
    }

    template<typename R>
    solver(solver<R> other) : is_X(other.is_X) {
        for (uint32_t x = 0; x < N; x++) {
            dp[x] = other.dp[x];
        }
    }

    solver() = default;
};

#include <map>

template<typename T>
std::ostream &operator<<(std::ostream &output, const solver<T> &s) {
    output << "solutions: {\n";
    std::map<T, std::vector<uint32_t>> m;
    for(uint32_t x = 0; x < N; x++) {
        m[s.dp[x]].push_back(x);
    }
    std::cout << m.size() << "\n";
    for(auto [val, xs] : m) {
        std::cout << (uint32_t)val << ": ";
        for(uint32_t x : xs) {
            std::cout << x << ", ";
        }
        std::cout << "\n";
    }
    //for (uint32_t x = 0; x < N; x++) {
    //    output << "(" << x << "->" << static_cast<uint64_t>(s.dp[x]) << ")\n";
    //}
    return output << "}";
}

template<typename T>
solver<T> operator>>(solver<T> s, uint64_t k) {
    for (uint32_t x = 0; x < N; x++) {
        s.dp[x] >>= k;
    }
    return s;
}

template<typename T>
solver<T> operator<<(solver<T> s, uint64_t k) {
    for (uint32_t x = 0; x < N; x++) {
        s.dp[x] <<= k;
    }
    return s;
}

template<typename T>
solver<T> operator~(solver<T> s) {
    for (uint32_t x = 0; x < N; x++) {
        s.dp[x] = ~s.dp[x];
    }
    return s;
}

#define ADD_OOP(oop)                                              \
    template<typename T>                                          \
    solver<T> operator oop(solver<T> lhs, const solver<T> &rhs) { \
        lhs.is_X |= rhs.is_X;                                     \
        for (uint32_t x = 0; x < N; x++) {                        \
            lhs.dp[x] = lhs.dp[x] oop rhs.dp[x];                  \
        }                                                         \
        return lhs;                                               \
    }

//if (lhs.is_X) {                                         \
    if (rhs.is_X) {                                       \
        lhs.val = lhs.val oop rhs.val;                    \
        for (uint32_t x = 0; x < N; x++) {                \
            lhs.dp[x] = lhs.dp[x] oop rhs.dp[x];          \
        }                                                 \
        return lhs;                                       \
    } else {                                              \
        lhs.val = lhs.val oop rhs.val;                    \
        for (uint32_t x = 0; x < N; x++) {              \
            lhs.dp[x] = lhs.dp[x] oop rhs.val;            \
        }                                                 \
        return lhs;                                       \
    }                                                     \
} else if (rhs.is_X) {                                    \
    return rhs oop lhs;                                   \
} else {                                                  \
    lhs.val = lhs.val oop rhs.val;                        \
    return lhs;                                           \
}                                                         \
}

ADD_OOP(+)
ADD_OOP(-)
ADD_OOP(*)
ADD_OOP(&)
ADD_OOP(^)
ADD_OOP(|)

template<typename T>
solver<T> choose(solver<T> e, solver<T> f, solver<T> g) {
    return (e & f) ^ (~e & g);

    solver<T> res;
    res.is_X = e.is_X || f.is_X || g.is_X;
    for (uint32_t x = 0; x < N; x++) {
        res.dp[x] = (e.dp[x] & f.dp[x]) ^ (~e.dp[x] & g.dp[x]);
    }
    return res;
}

template<typename T>
solver<T> rotr(solver<T> x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

template<typename T>
solver<T> majority(solver<T> a, solver<T> b, solver<T> c) {
    return (a & (b | c)) | (b & c);
}

template<typename T>
solver<T> sig0(solver<T> x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

template<typename T>
solver<T> sig1(solver<T> x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}