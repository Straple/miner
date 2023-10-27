#pragma once

#include "../fast_string.hpp"
#include <string>

fast_string sha256(const std::string &str);

fast_string sha256(const fast_string &str);