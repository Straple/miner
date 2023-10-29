#include "fast_string.hpp"

#include "assert.hpp"
#include "constants.hpp"
#include <cstring>

// Работает как std::memcmp, только сравнивает с конца.
// Сравнивает по 8 байт
int64_t memcmp_reverse(const void *buf1, const void *buf2, size_t n) {

    // uint8_t
    {
        auto a = reinterpret_cast<const uint8_t *>(buf1) + n;
        auto b = reinterpret_cast<const uint8_t *>(buf2) + n;

        size_t k = n % sizeof(uint64_t);
        n -= k;

        while (k > 0) {
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
        n /= sizeof(int64_t);

        auto a = reinterpret_cast<const uint64_t *>(buf1) + n;
        auto b = reinterpret_cast<const uint64_t *>(buf2) + n;

        while (n > 0) {
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

fast_string::fast_string(const std::string &str) {
    ASSERT(str.size() <= FAST_STRING_LEN, "str size more than FAST_STRING_LEN: " + std::to_string(str.size()) + " " + str);
    len = str.size();
    std::memcpy(data, str.data(), len);
}

fast_string::fast_string(uint32_t size) : len(size) {
    ASSERT(size <= FAST_STRING_LEN, "out of range");
}

fast_string::fast_string(const char *str) {
    while (len < FAST_STRING_LEN && str[len] != '\0') {
        data[len] = str[len];
        len++;
    }
    ASSERT(str[len] == '\0', "out of range");
}

std::string fast_string::to_str() const {
    return std::string(data, len);
}

char *fast_string::c_str() {
    return data;
}

const char *fast_string::c_str() const {
    return data;
}

char &fast_string::back() {
    ASSERT(len != 0, "is empty");
    return data[len - 1];
}

const char &fast_string::back() const {
    ASSERT(len != 0, "is empty");
    return data[len - 1];
}

char *fast_string::begin() {
    return data;
}

char *fast_string::end() {
    return data + len;
}

const char *fast_string::begin() const {
    return data;
}

const char *fast_string::end() const {
    return data + len;
}

uint32_t fast_string::size() const {
    return len;
}

fast_string &fast_string::operator+=(char symbol) {
    ASSERT(len < FAST_STRING_LEN, "failed to push");
    data[len] = symbol;
    len++;
    return *this;
}

fast_string &fast_string::operator+=(const fast_string &str) {
    ASSERT(len + str.len <= FAST_STRING_LEN, "failed addition");
    std::memcpy(data + len, str.data, str.len);
    len += str.len;
    return *this;
}

char &fast_string::operator[](uint32_t index) {
    ASSERT(index < len, "out of range");
    return data[index];
}

const char &fast_string::operator[](uint32_t index) const {
    ASSERT(index < len, "out of range");
    return data[index];
}

bool operator<(const fast_string &lhs, const fast_string &rhs) {
    if (lhs.size() != rhs.size()) {
        return lhs.size() < rhs.size();
    } else {
        return memcmp_reverse(lhs.data, rhs.data, lhs.len) < 0;
    }
}

bool operator==(const fast_string &lhs, const fast_string &rhs) {
    if (lhs.len != rhs.len) {
        return false;
    } else {
        return memcmp_reverse(lhs.data, rhs.data, lhs.len) == 0;
    }
}

bool operator!=(const fast_string &lhs, const fast_string &rhs) {
    return !(lhs == rhs);
}

std::ostream &operator<<(std::ostream &output, const fast_string &str) {
    for (uint32_t index = 0; index < str.len; index++) {
        output << str[index];
    }
    return output;
}

fast_string operator+(fast_string lhs, const fast_string &rhs) {
    return lhs += rhs;
}

uint32_t fast_string::builtin_ctz() const {
    int k = len - 1;
    for (; k >= 0 && data[k] == 0; k--) {}
    return (len - 1 - k) * 8 + (__builtin_clz(static_cast<uint8_t>(data[k])) - 24);
}