#include "CsvLogger.h"

#include <fstream>
#include <filesystem>

void CsvLogger::createHeaderIfNeeded()
{
    const std::string filename = "logs/metrics.csv";

    // Create logs directory if it doesn't exist
    std::filesystem::create_directories("logs");

    // Check whether the file already exists
    std::ifstream inputFile(filename);

    if (inputFile.good())
    {
        inputFile.close();
        return;
    }

    inputFile.close();

    // Create file and write CSV header
    std::ofstream outputFile(filename);

    if (!outputFile.is_open())
    {
        return;
    }

    outputFile
        << "timestamp,"
        << "client_id,"
        << "hostname,"
        << "kernel_version,"
        << "cpu_usage,"
        << "memory_usage,"
        << "disk_usage,"
        << "process_count,"
        << "uptime_seconds\n";

    outputFile.close();
}


void CsvLogger::logMetrics(const SystemData& data)
{
    const std::string filename = "logs/metrics.csv";

    createHeaderIfNeeded();

    std::ofstream outputFile(
        filename,
        std::ios::app
    );

    if (!outputFile.is_open())
    {
        return;
    }

    outputFile
        << data.timestamp << ','
        << data.clientId << ','
        << data.hostname << ','
        << data.kernelVersion << ','
        << data.cpuUsage << ','
        << data.memoryUsage << ','
        << data.diskUsage << ','
        << data.processCount << ','
        << data.uptimeSeconds
        << '\n';

    outputFile.close();
}
