#include "ConfigLoader.h"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

Config ConfigLoader::load(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Could not open configuration file: " + filename
        );
    }

    json data;
    file >> data;

    Config config;

    config.serverHost = data["server"]["host"];
    config.serverPort = data["server"]["port"];

    config.clientInterval = data["client"]["interval"];
    config.retryInterval = data["client"]["retry_interval"];

    config.cpuWarning = data["thresholds"]["cpu_warning"];
    config.cpuCritical = data["thresholds"]["cpu_critical"];

    config.memoryWarning = data["thresholds"]["memory_warning"];
    config.memoryCritical = data["thresholds"]["memory_critical"];

    config.diskWarning = data["thresholds"]["disk_warning"];
    config.diskCritical = data["thresholds"]["disk_critical"];

    config.heartbeatTimeout = data["heartbeat"]["timeout"];

    return config;
}
