#pragma once

#include "assert.hpp"
#include "constants.hpp"
#include <cstring>

class fast_string {
    uint32_t len = 0;
    char data[FAST_STRING_LEN]{};

public:
    fast_string() = default;

    fast_string(const std::string &str);

    fast_string(const char *str);

    fast_string(uint32_t size);

    [[nodiscard]] std::string to_str() const;

    char *c_str();
    [[nodiscard]] const char *c_str() const;

    char &back();
    [[nodiscard]] const char &back() const;

    char *begin();
    char *end();
    [[nodiscard]] const char *begin() const;
    [[nodiscard]] const char *end() const;

    [[nodiscard]] uint32_t size() const;

    fast_string &operator+=(char symbol);

    fast_string &operator+=(const fast_string &str);

    char &operator[](uint32_t index);

    const char &operator[](uint32_t index) const;

    friend bool operator<(const fast_string &lhs, const fast_string &rhs);

    friend bool operator==(const fast_string &lhs, const fast_string &rhs);

    friend std::ostream &operator<<(std::ostream &output, const fast_string &str);

    uint32_t builtin_ctz() const;
};

bool operator!=(const fast_string &lhs, const fast_string &rhs);

fast_string operator+(fast_string lhs, const fast_string &rhs);