#include "Dashboard.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>

void Dashboard::display(
    const std::vector<ClientState>& clients
)
{
    // Clear terminal screen
    std::cout << "\033[2J\033[H";

    std::cout << "==============================================\n";
    std::cout << "           LINUX SYSTEM MONITOR\n";
    std::cout << "==============================================\n\n";

    std::cout
        << std::left
        << std::setw(15) << "Client ID"
        << std::setw(15) << "Hostname"
        << std::setw(10) << "CPU %"
        << std::setw(12) << "Memory %"
        << std::setw(10) << "Disk %"
        << std::setw(12) << "Processes"
        << std::setw(12) << "Status"
        << "\n";

    std::cout
        << "--------------------------------------------------------------------------\n";

    if (clients.empty())
    {
        std::cout
            << "\nNo clients connected.\n";
    }
    else
    {
        for (const auto& client : clients)
        {
            std::cout
                << std::left
                << std::setw(15)
                << client.clientId

                << std::setw(15)
                << client.hostname

                << std::setw(10)
                << std::fixed
                << std::setprecision(1)
                << client.latestMetrics.cpuUsage

                << std::setw(12)
                << client.latestMetrics.memoryUsage

                << std::setw(10)
                << client.latestMetrics.diskUsage

                << std::setw(12)
                << client.latestMetrics.processCount

                << std::setw(12)
                << client.status

                << "\n";
        }
    }

    std::cout
        << "\n--------------------------------------------------------------------------\n";

    std::cout
        << "Total Clients: "
        << clients.size()
        << "\n";

    std::cout
        << "Dashboard refresh interval: 2 seconds\n";
}

// DASHBOARD REFRESH

void Dashboard::startRefresh(
    ClientRegistry& registry,
    std::atomic<bool>& running
)
{
    while (running)
    {
        // Get current clients
        std::vector<ClientState> clients =
            registry.getAll();

        // Display dashboard
        display(clients);

        // Refresh every 2 seconds
        for (int i = 0; i < 20 && running; ++i)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }
    }

    std::cout
        << "\nDashboard stopped.\n";
}
