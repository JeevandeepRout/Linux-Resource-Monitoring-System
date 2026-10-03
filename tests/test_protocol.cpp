#include <iostream>
#include <cassert>
#include <nlohmann/json.hpp>
#include "../common/SystemData.h"

using json = nlohmann::json;

int main() {

    SystemData data;

    data.clientId = "TEST-CLIENT";
    data.hostname = "Ubuntu";
    data.kernelVersion = "6.8.0";
    data.cpuUsage = 25.5;
    data.memoryUsage = 65.2;
    data.diskUsage = 40.1;
    data.processCount = 300;
    data.uptimeSeconds = 10000;
    data.timestamp = 1790943339;

    json message;

    message["type"] = "metrics";
    message["client_id"] = data.clientId;
    message["hostname"] = data.hostname;
    message["kernel_version"] = data.kernelVersion;
    message["cpu_usage"] = data.cpuUsage;
    message["memory_usage"] = data.memoryUsage;
    message["disk_usage"] = data.diskUsage;
    message["process_count"] = data.processCount;
    message["uptime_seconds"] = data.uptimeSeconds;
    message["timestamp"] = data.timestamp;

    std::cout << "Generated JSON:\n";
    std::cout << message.dump(4) << "\n";

    assert(message["type"] == "metrics");
    assert(message["client_id"] == "TEST-CLIENT");
    assert(message["hostname"] == "Ubuntu");
    assert(message["kernel_version"] == "6.8.0");
    assert(message["cpu_usage"] == 25.5);
    assert(message["memory_usage"] == 65.2);
    assert(message["disk_usage"] == 40.1);
    assert(message["process_count"] == 300);
    assert(message["uptime_seconds"] == 10000);
    assert(message["timestamp"] == 1790943339);

    std::cout << "\nProtocol test passed!\n";

    return 0;
}
