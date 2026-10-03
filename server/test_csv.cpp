#include "CsvLogger.h"

int main()
{
    CsvLogger logger;

    SystemData data;

    data.clientId = "PC-TEST";
    data.hostname = "Ubuntu";
    data.kernelVersion = "7.0.0-test";

    data.cpuUsage = 25.5;
    data.memoryUsage = 65.2;
    data.diskUsage = 40.1;

    data.processCount = 300;
    data.uptimeSeconds = 10000;
    data.timestamp = 1790943339;

    logger.logMetrics(data);

    return 0;
}
