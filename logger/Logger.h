#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger
{
public:
    enum class LogLevel
    {
        INFO = 0,
        WARNING = 1,
        ERROR = 2
    };

    Logger(const std::string& filename, LogLevel defaultLevel);
    ~Logger();

    void log(const std::string& message, LogLevel level);
    void setCurrentLevel(LogLevel level);
    LogLevel getLogLevel();

private:
    std::string levelToString(LogLevel level) const;
    std::string currentDateTime() const;

    std::ofstream logFile;
    LogLevel currentLevel;
    std::mutex fileMutex;
};

#endif
