#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>

class Logger
{
public:
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    static void writeLog(
        const std::string& level,
        const std::string& message
    );

    static std::mutex logMutex;
};

#endif
