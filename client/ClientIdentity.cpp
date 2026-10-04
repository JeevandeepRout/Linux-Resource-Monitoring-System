#include "ClientIdentity.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <random>

namespace fs = std::filesystem;

// Get Client ID
std::string ClientIdentity::getClientId()
{
    return loadOrCreateId();
}

// Load existing ID or create a new one
std::string ClientIdentity::loadOrCreateId()
{
    const char* homeDirectory = std::getenv("HOME");

    if (homeDirectory == nullptr)
    {
        throw std::runtime_error(
            "HOME environment variable not found"
        );
    }

    // ~/.linux-monitor
    fs::path directory =
        fs::path(homeDirectory) / ".linux-monitor";

    // Create directory if it doesn't exist
    fs::create_directories(directory);

    // ~/.linux-monitor/client_id
    fs::path idFile =
        directory / "client_id";

    // Try to read existing ID
    std::ifstream inputFile(idFile);

    if (inputFile)
    {
        std::string clientId;

        std::getline(inputFile, clientId);

        if (!clientId.empty())
        {
            return clientId;
        }
    }

    // Generate new ID
    std::random_device randomDevice;

    std::mt19937 generator(randomDevice());

    std::uniform_int_distribution<int> distribution(
        100000,
        999999
    );

    std::string clientId =
        "PC-" + std::to_string(distribution(generator));

    // Save ID
    std::ofstream outputFile(idFile);

    if (!outputFile)
    {
        throw std::runtime_error(
            "Failed to save client ID"
        );
    }

    outputFile << clientId;

    return clientId;
}
