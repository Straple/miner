
#include "logger.hpp"
#include <iomanip>

std::string get_now_datetime() {
    // uint64_t nanosecond = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count() % 1'000'000'000;

    std::time_t t = std::time(0);
    std::tm *now = std::localtime(&t);

    std::stringstream ss;
    ss.fill('0');

    ss << (now->tm_year + 1900) << '-'
       << std::setw(2) << now->tm_mon + 1 << '-'
       << std::setw(2) << now->tm_mday << ' '
       << std::setw(2) << now->tm_hour << ':'
       << std::setw(2) << now->tm_min << ':'
       << std::setw(2) << now->tm_sec;
    // << "::" << std::setw(9) << nanosecond;
    return ss.str();
}

void Logger::print(std::string message) {
    output << '[' << get_now_datetime() << "] " << message << std::endl;
}