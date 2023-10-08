
#include "logger.hpp"

std::string get_now_datetime() {
    uint64_t nanosecond = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                  std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point())
                                  .count() %
                          1'000'000'000;
    std::time_t t = std::time(0);
    std::tm *now = std::localtime(&t);
    std::stringstream ss;
    ss << (now->tm_year + 1900) << '-'
       << now->tm_mon + 1 << '-'
       << now->tm_mday << '.' << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << "::";
    for (uint64_t x = 1'000'000'000; x > nanosecond; x /= 10) {
        ss << '0';
    }
    ss << nanosecond;
    return ss.str();
}

void Logger::print(std::string message) {
    output << '[' << get_now_datetime() << "] " << message << std::endl;
}