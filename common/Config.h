#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config
{
    std::string serverHost;
    int serverPort;

    int clientInterval;
    int retryInterval;

    double cpuWarning;
    double cpuCritical;

    double memoryWarning;
    double memoryCritical;

    double diskWarning;
    double diskCritical;

    int heartbeatTimeout;
};

#endif
