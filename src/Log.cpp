#include "Log.h"

void Log::logger(LogLevel level, const std::string& message) {
    switch (level)
    {
    case LogLevel::INFO:
        std::clog << GREEN << "INFO: " << message << RESET_COLOR << std::endl;
        break;
    case LogLevel::DEBUG:
        std::clog << WHITE << "DEBUG: " << message << RESET_COLOR << std::endl;
        break;
    case LogLevel::WARNING:
        std::clog << YELLOW << "WARNING: " << message << RESET_COLOR << std::endl;
        break;
    case LogLevel::ERROR:
        std::cerr << RED << "ERROR: " << message << RESET_COLOR << std::endl;
        break;

    default:
        std::clog << YELLOW << "Unknown log type" << RESET_COLOR << std::endl;
        break;
    }
}