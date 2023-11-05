#pragma once

#include <cstdint>

constexpr int SOLVER_LEN = 128;

template<typename T>
struct solver {

    // data[x] = если подставить x, то получим такое значение тут
    //std::vector<T> data = std::vector<T>(SOLVER_LEN);
    T data[SOLVER_LEN];

    solver() {
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            data[index] = 0;
        }
    }

    solver(T val) {
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            data[index] = val;
        }
    }

    template<typename T2>
    solver(const solver<T2> &other) {
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            data[index] = other[index];
        }
    }

    const T &operator[](uint32_t index) const {
        return data[index];
    }

    T &operator[](uint32_t index) {
        return data[index];
    }
};

template<typename T>
solver<T> operator~(solver<T> s) {
    for (uint32_t index = 0; index < SOLVER_LEN; index++) {
        s[index] = ~s[index];
    }
    return s;
}

template<typename T>
solver<T> operator>>(solver<T> s, const uint32_t shift) {
    for (uint32_t index = 0; index < SOLVER_LEN; index++) {
        s[index] = s[index] >> shift;
    }
    return s;
}

template<typename T>
solver<T> operator<<(solver<T> s, const uint32_t shift) {
    for (uint32_t index = 0; index < SOLVER_LEN; index++) {
        s[index] = s[index] << shift;
    }
    return s;
}

template<typename T>
inline bool operator==(const solver<T> lhs, const solver<T> rhs) {
    for (uint32_t index = 0; index < SOLVER_LEN; index++) {
        if (lhs[index] != rhs[index]) {
            return false;
        }
    }
    return true;
}

template<typename T>
inline bool operator!=(const solver<T> lhs, const solver<T> rhs) {
    return !(lhs == rhs);
}

#define ADD_OOP(oop)                                              \
    template<typename T>                                          \
    solver<T> operator oop(solver<T> lhs, const solver<T> &rhs) { \
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {   \
            lhs[index] = lhs[index] oop rhs[index];               \
        }                                                         \
        return lhs;                                               \
    }

ADD_OOP(+)
ADD_OOP(&)
ADD_OOP(^)
ADD_OOP(|)

#define ADD_OOP2(oop)                                              \
    template<typename T>                                          \
    solver<T> operator oop(solver<T> lhs, const solver<T> &rhs) { \
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {   \
            lhs[index] oop rhs[index];               \
        }                                                         \
        return lhs;                                               \
    }

ADD_OOP2(+=)
ADD_OOP2(&=)
ADD_OOP2(^=)
ADD_OOP2(|=)