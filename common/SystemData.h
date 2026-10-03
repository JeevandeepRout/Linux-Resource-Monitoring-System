#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H

#include <string>

struct SystemData {
    std::string clientId;
    std::string hostname;
    std::string kernelVersion;

    double cpuUsage;
    double memoryUsage;
    double diskUsage;

    int processCount;

    double uptimeSeconds;

    long long timestamp;
};

#endif