#include "Logger.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

std::mutex Logger::logMutex;


// ============================================================
// Write Log
// ============================================================

void Logger::writeLog(
    const std::string& level,
    const std::string& message)
{
    std::lock_guard<std::mutex> lock(logMutex);

    std::ofstream logFile(
        "logs/server.log",
        std::ios::app
    );

    if (!logFile.is_open())
    {
        std::cerr
            << "Failed to open log file.\n";

        return;
    }


    // Get current time

    auto now =
        std::chrono::system_clock::now();

    std::time_t currentTime =
        std::chrono::system_clock::to_time_t(now);


    logFile
        << "["
        << std::put_time(
               std::localtime(&currentTime),
               "%Y-%m-%d %H:%M:%S"
           )
        << "] "
        << "["
        << level
        << "] "
        << message
        << '\n';

    logFile.close();
}


// ============================================================
// INFO
// ============================================================

void Logger::info(
    const std::string& message)
{
    writeLog("INFO", message);
}


// ============================================================
// WARNING
// ============================================================

void Logger::warning(
    const std::string& message)
{
    writeLog("WARNING", message);
}


// ============================================================
// ERROR
// ============================================================

void Logger::error(
    const std::string& message)
{
    writeLog("ERROR", message);
}
