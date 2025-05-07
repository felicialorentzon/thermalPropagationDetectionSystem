#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>

enum class LogLevel {
    INFO,
    DEBUG,
    WARNING,
    ERROR,
};

class Log {
public:
    void logger(LogLevel level, const std::string& message);
private:
    const std::string RESET_COLOR = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string WHITE = "\033[37m";
    const std::string GREEN = "\033[32m";
};

#endif