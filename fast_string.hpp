#pragma once

#include "assert.hpp"
#include "constants.hpp"
#include <cstring>

// Работает как std::memcmp, только сравнивает с конца.
// Сравнивает по 8 байт
int64_t memcmp_reverse(const void *buf1, const void *buf2, size_t n);

class fast_string {
    uint32_t len = 0;
    char data[FAST_STRING_LEN];

public:
    fast_string(std::string str);

    uint16_t size() const;

    char operator[](uint32_t index);

    const char operator[](uint32_t index) const;

    friend bool operator<(const fast_string &lhs, const fast_string &rhs);
};