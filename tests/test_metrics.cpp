#include <iostream>
#include <cassert>
#include "../client/SystemMonitor.h"

int main() {
    SystemMonitor monitor;

    double cpu = monitor.getCpuUsage();
    double memory = monitor.getMemoryUsage();
    double disk = monitor.getDiskUsage();
    int processes = monitor.getProcessCount();

    std::cout << "CPU Usage: " << cpu << "%\n";
    std::cout << "Memory Usage: " << memory << "%\n";
    std::cout << "Disk Usage: " << disk << "%\n";
    std::cout << "Process Count: " << processes << "\n";

    assert(cpu >= 0.0 && cpu <= 100.0);
    assert(memory >= 0.0 && memory <= 100.0);
    assert(disk >= 0.0 && disk <= 100.0);
    assert(processes > 0);

    std::cout << "\nMetric tests passed!\n";

    return 0;
}
