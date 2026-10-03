#include <iostream>
#include "ConfigLoader.h"

int main()
{
    try
    {
        Config config = ConfigLoader::load("config/config.json");

        std::cout << "Server Host: "
                  << config.serverHost << '\n';

        std::cout << "Server Port: "
                  << config.serverPort << '\n';

        std::cout << "Client Interval: "
                  << config.clientInterval << '\n';

        std::cout << "CPU Warning: "
                  << config.cpuWarning << '\n';

        std::cout << "CPU Critical: "
                  << config.cpuCritical << '\n';

        std::cout << "Heartbeat Timeout: "
                  << config.heartbeatTimeout << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
