

#include "SystemMonitor.h"
#include <unistd.h>
#include <stdexcept>
#include <fstream>
#include <string>
#include <sys/statvfs.h>
#include <filesystem>
#include <cctype>
#include <chrono>
#include <thread>

using namespace std;

struct CpuTimes {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
};

double SystemMonitor::getMemoryUsage() {
    std::ifstream file("/proc/meminfo");

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/meminfo");
    }

    std::string label;
    long long value;
    std::string unit;

    long long total = 0;
    long long available = 0;

    while (file >> label >> value >> unit) {

        if (label == "MemTotal:") {
            total = value;
        }

        if (label == "MemAvailable:") {
            available = value;
        }

        if (total > 0 && available > 0) {
            break;
        }
    }

    if (total == 0) {
        throw std::runtime_error("Failed to read memory information");
    }

    long long used = total - available;

    return (static_cast<double>(used) / total) * 100.0;
}

double SystemMonitor::getDiskUsage() {
    struct statvfs stat;

    if (statvfs("/", &stat) != 0) {
        throw std::runtime_error("Failed to get disk information");
    }

    unsigned long long total =
        static_cast<unsigned long long>(stat.f_blocks) *
        stat.f_frsize;

    unsigned long long available =
        static_cast<unsigned long long>(stat.f_bavail) *
        stat.f_frsize;

    unsigned long long used = total - available;

    return (static_cast<double>(used) / total) * 100.0;
}

int SystemMonitor::getProcessCount() {

    int count = 0;

    for (const auto& entry :
         std::filesystem::directory_iterator("/proc")) {

        if (!entry.is_directory()) {
            continue;
        }

        std::string name = entry.path().filename().string();

        bool isNumeric = !name.empty();

        for (char c : name) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                isNumeric = false;
                break;
            }
        }

        if (isNumeric) {
            count++;
        }
    }

    return count;
}

double SystemMonitor::getUptime() {

    std::ifstream file("/proc/uptime");

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/uptime");
    }

    double uptime;

    file >> uptime;

    if (file.fail()) {
        throw std::runtime_error("Failed to read uptime");
    }

    return uptime;
}

CpuTimes readCpuTimes() {

    std::ifstream file("/proc/stat");

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/stat");
    }

    std::string cpu;

    CpuTimes times{};

    file >> cpu
         >> times.user
         >> times.nice
         >> times.system
         >> times.idle
         >> times.iowait
         >> times.irq
         >> times.softirq
         >> times.steal;

    if (file.fail()) {
        throw std::runtime_error("Failed to read CPU information");
    }

    return times;
}

double SystemMonitor::getCpuUsage() {

    CpuTimes first = readCpuTimes();

    std::this_thread::sleep_for(
        std::chrono::seconds(1)
    );

    CpuTimes second = readCpuTimes();

    unsigned long long idleFirst =
        first.idle + first.iowait;

    unsigned long long idleSecond =
        second.idle + second.iowait;

    unsigned long long totalFirst =
        first.user +
        first.nice +
        first.system +
        first.idle +
        first.iowait +
        first.irq +
        first.softirq +
        first.steal;

    unsigned long long totalSecond =
        second.user +
        second.nice +
        second.system +
        second.idle +
        second.iowait +
        second.irq +
        second.softirq +
        second.steal;

    unsigned long long totalDelta =
        totalSecond - totalFirst;

    unsigned long long idleDelta =
        idleSecond - idleFirst;

    if (totalDelta == 0) {
        return 0.0;
    }

    double usage =
        (static_cast<double>(totalDelta - idleDelta)
        / totalDelta) * 100.0;

    return usage;
}

string SystemMonitor::getHostname() {
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        throw runtime_error("Failed to get hostname");
    }

    return string(hostname);
}

string SystemMonitor::getKernelVersion() {

    ifstream file("/proc/sys/kernel/osrelease");

    if (!file.is_open()) {
        throw std::runtime_error(
            "Failed to open /proc/sys/kernel/osrelease"
        );
    }

    string version;

    getline(file, version);

    if (version.empty()) {
        throw std::runtime_error(
            "Failed to read kernel version"
        );
    }

    return version;
}

SystemData SystemMonitor::collect() {

    SystemData data;

    data.hostname = getHostname();
    data.kernelVersion = getKernelVersion();

    data.cpuUsage = getCpuUsage();
    data.memoryUsage = getMemoryUsage();
    data.diskUsage = getDiskUsage();

    data.processCount = getProcessCount();

    data.uptimeSeconds = getUptime();

    return data;
}



