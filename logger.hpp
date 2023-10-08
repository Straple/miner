#pragma once

#include <chrono>
#include <fstream>
#include <sstream>

// TODO: сделать более читабельное логирование для каждого потока
// а именно выписывать для каждого потока в отдельный файл лог с его событиями
// выписывать точное время и что случилось

class Logger {
    std::ofstream output;

public:
    Logger() = default;
    Logger(const std::string &filename) : output(filename) {
    }
    void print(std::string message);
};