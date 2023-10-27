#pragma once

#include "fast_string.hpp"

fast_string byte_to_hex(unsigned char byte);

fast_string integer_to_hex(uint64_t integer, uint64_t len);

fast_string integer_to_bytes(uint64_t integer, uint64_t len);

uint64_t hex_to_integer(char hex_code);

uint64_t hex_to_integer(fast_string hex_str);

fast_string hex_to_bytes(const fast_string &hex_str);

std::string hex_to_bytes(const std::string &hex_str);

fast_string bytes_to_hex(const fast_string &bytes);

std::string bytes_to_hex(const std::string &bytes);

fast_string hex_multiply(const fast_string &lhs, const fast_string &rhs);

fast_string byte_reverse_in_hex(fast_string hex_str);

fast_string reverse_str(fast_string str);

uint32_t reverse_bytes(uint32_t x);