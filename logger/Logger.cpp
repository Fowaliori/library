#include "Logger.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <stdexcept>

Logger::Logger(const std::string& filename, LogLevel defaultLevel)
    : currentLevel(defaultLevel)
{
    logFile.open(filename, std::ios::app);

    if (!logFile.is_open())
        throw std::runtime_error("Не могу открыть файл");
}

Logger::~Logger()
{
    if (logFile.is_open())
        logFile.close();
}

void Logger::setCurrentLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(fileMutex);
    currentLevel = level;
}

Logger::LogLevel Logger::getLogLevel()
{
    std::lock_guard<std::mutex> lock(fileMutex);
    return currentLevel;
}

void Logger::log(const std::string& message, LogLevel level)
{
    std::lock_guard<std::mutex> lock(fileMutex);

    if (level < currentLevel)
        return;

    if (!logFile.is_open())
        throw std::runtime_error("Файл не открыт");

    logFile << "[" << currentDateTime() << "] "
            << "[" << levelToString(level) << "] "
            << message << std::endl;

    if (!logFile.good())
        throw std::runtime_error("Ошибка записи в файл");
}

std::string Logger::levelToString(LogLevel level) const
{
    switch (level)
    {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::currentDateTime() const
{
    auto now = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(now);
    struct tm* tm = std::localtime(&t);

    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buf);
}
