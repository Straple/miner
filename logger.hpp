#pragma once

#include <chrono>
#include <fstream>
#include <sstream>

class Logger {
    std::ofstream output;

public:
    Logger() = default;
    Logger(const std::string &filename) : output(filename) {
    }
    void print(std::string message);
};