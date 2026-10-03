
#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H
using namespace std;

#include <string>
#include "../common/SystemData.h"

class SystemMonitor {
public:
    string getHostname();
    double getMemoryUsage();
    double getDiskUsage();
    int getProcessCount();
    double getUptime();
    string getKernelVersion();
    double getCpuUsage();
    SystemData collect();
};

#endif
