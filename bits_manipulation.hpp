#pragma once

#include <string>

std::string byte_to_hex(unsigned char byte);

std::string integer_to_hex(uint64_t integer, uint64_t len);

std::string integer_to_bytes(uint64_t integer, uint64_t len);

uint64_t hex_to_integer(char hex_code);

uint64_t hex_to_integer(std::string hex_str);

std::string hex_to_bytes(const std::string &hex_str);

std::string bytes_to_hex(const std::string &bytes);

std::string hex_multiply(const std::string &lhs, const std::string &rhs);

std::string byte_reverse_in_hex(std::string hex_str);

std::string reverse_str(std::string str);

uint32_t reverse_bytes(uint32_t x);