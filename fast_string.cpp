#include "fast_string.hpp"

#include "assert.hpp"
#include "constants.hpp"
#include <cstring>

// Работает как std::memcmp, только сравнивает с конца.
// Сравнивает по 8 байт
int64_t memcmp_reverse(const void *buf1, const void *buf2, size_t n) {

    // uint8_t
    {
        size_t k = n % sizeof(uint64_t);

        n -= k;

        auto a = reinterpret_cast<const uint8_t *>(buf1) + k;
        auto b = reinterpret_cast<const uint8_t *>(buf2) + k;

        while (k) {
            --k;
            --a;
            --b;

            if (*a != *b) {
                return static_cast<int64_t>(*a) - *b;
            }
        }
    }

    // uint64_t
    {
        n /= sizeof(uint64_t);

        auto a = reinterpret_cast<const uint64_t *>(buf1) + n;
        auto b = reinterpret_cast<const uint64_t *>(buf2) + n;

        while (n) {
            --n;
            --a;
            --b;

            if (*a != *b) {

                auto c = reinterpret_cast<const uint32_t *>(a) + 1;
                auto d = reinterpret_cast<const uint32_t *>(b) + 1;

                if (*c == *d) {
                    --c;
                    --d;
                }
                return static_cast<int64_t>(*c) - *d;
            }
        }
    }

    return 0;
}

fast_string::fast_string(std::string str) {
    ASSERT(str.size() <= FAST_STRING_LEN, "str size more then FAST_STRING_LEN");

    len = str.size();
    std::memcpy(data, str.data(), len);
}

uint16_t fast_string::size() const {
    return len;
}

char fast_string::operator[](uint32_t index) {
    ASSERT(index < len, "out of range");
    return data[index];
}

const char fast_string::operator[](uint32_t index) const {
    ASSERT(index < len, "out of range");
    return data[index];
}

bool operator<(const fast_string &lhs, const fast_string &rhs) {
    if (lhs.size() != rhs.size()) {
        return lhs.size() < rhs.size();
    } else {
        return memcmp_reverse(lhs.data + lhs.len - 1, rhs.data + rhs.len - 1, lhs.len);
    }
}
