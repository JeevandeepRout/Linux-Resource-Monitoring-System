#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>

#include "NetworkClient.h"
#include "SystemMonitor.h"
#include "ClientIdentity.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;


// ============================================================
// SHUTDOWN FLAG
// ============================================================

volatile sig_atomic_t shutdownRequested = 0;


// ============================================================
// CTRL+C HANDLER
// ============================================================

void handleSignal(int signal)
{
    if (signal == SIGINT)
    {
        shutdownRequested = 1;
    }
}


// ============================================================
// OLD CODE — PHASE 24
// Simple "Hello from client" TCP test
// Kept in comment mode.
// ============================================================

/*

int main()
{
    NetworkClient client;

    if (!client.connectToServer("127.0.0.1", 5000))
    {
        std::cerr << "Could not connect to server\n";
        return 1;
    }

    if (client.sendMessage("Hello from client\n"))
    {
        std::cout << "Test message sent successfully\n";
    }
    else
    {
        std::cerr << "Failed to send test message\n";
    }

    client.disconnect();

    return 0;
}

*/


// ============================================================
// PHASE 47
// Client Reconnection
//
// If the connection fails:
//     Wait 5 seconds
//     Try again
//
// If the connection is lost:
//     Disconnect
//     Try reconnecting
// ============================================================

int main()
{
    try
    {
        // ----------------------------------------------------
        // Register Ctrl+C handler
        // ----------------------------------------------------

        std::signal(
            SIGINT,
            handleSignal
        );


        // ----------------------------------------------------
        // Create objects
        // ----------------------------------------------------

        SystemMonitor monitor;

        NetworkClient client;

        ClientIdentity identity;


        // ----------------------------------------------------
        // Server configuration
        // ----------------------------------------------------

        const std::string serverHost =
            "127.0.0.1";

        const int serverPort =
            5000;


        // ----------------------------------------------------
        // Reconnection interval
        // ----------------------------------------------------

        const int retryInterval =
            5;


        // ----------------------------------------------------
        // Client identity
        // ----------------------------------------------------

        std::string clientId =
            identity.getClientId();


        std::cout
            << "Client ID: "
            << clientId
            << "\n";


        // ----------------------------------------------------
        // Main client loop
        // ----------------------------------------------------

        while (!shutdownRequested)
        {
            // =================================================
            // CONNECT TO SERVER
            // =================================================

            if (!client.isConnected())
            {
                std::cout
                    << "\nConnecting to server "
                    << serverHost
                    << ":"
                    << serverPort
                    << "...\n";


                if (!client.connectToServer(
                        serverHost,
                        serverPort))
                {
                    std::cout
                        << "Connection failed.\n";

                    std::cout
                        << "Retrying in "
                        << retryInterval
                        << " seconds...\n";


                    // Wait before reconnecting
                    for (
                        int i = 0;
                        i < retryInterval * 10 &&
                        !shutdownRequested;
                        ++i
                    )
                    {
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(100)
                        );
                    }

                    continue;
                }


                std::cout
                    << "Connection established.\n";
            }


            // =================================================
            // COLLECT SYSTEM INFORMATION
            // =================================================

            SystemData data =
                monitor.collect();


            // -------------------------------------------------
            // Set persistent client ID
            // -------------------------------------------------

            data.clientId =
                clientId;


            // -------------------------------------------------
            // Set timestamp
            // -------------------------------------------------

            data.timestamp =
                std::chrono::duration_cast<
                    std::chrono::seconds
                >(
                    std::chrono::system_clock::now()
                    .time_since_epoch()
                ).count();


            // =================================================
            // CONVERT SYSTEM DATA TO JSON
            // =================================================

            json message;

            message["type"] =
                "metrics";

            message["client_id"] =
                data.clientId;

            message["hostname"] =
                data.hostname;

            message["kernel_version"] =
                data.kernelVersion;

            message["cpu_usage"] =
                data.cpuUsage;

            message["memory_usage"] =
                data.memoryUsage;

            message["disk_usage"] =
                data.diskUsage;

            message["process_count"] =
                data.processCount;

            message["uptime_seconds"] =
                data.uptimeSeconds;

            message["timestamp"] =
                data.timestamp;


            // =================================================
            // DISPLAY JSON
            // =================================================

            std::cout
                << "\nJSON message:\n";

            std::cout
                << message.dump(4)
                << "\n";


            // =================================================
            // SEND JSON
            // =================================================

            std::string jsonMessage =
                message.dump() + "\n";


            if (client.sendMessage(
                    jsonMessage))
            {
                std::cout
                    << "\nJSON metrics sent successfully\n";
            }
            else
            {
                // =============================================
                // CONNECTION LOST
                // =============================================

                std::cerr
                    << "\nConnection to server lost.\n";

                std::cerr
                    << "Starting reconnection process...\n";


                client.disconnect();

                continue;
            }


            // =================================================
            // WAIT BEFORE NEXT METRIC
            // =================================================

            for (
                int i = 0;
                i < 50 &&
                !shutdownRequested;
                ++i
            )
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100)
                );
            }
        }


        // =====================================================
        // GRACEFUL CLIENT SHUTDOWN
        // =====================================================

        std::cout
            << "\nStopping client...\n";


        client.disconnect();


        std::cout
            << "Client shutdown complete.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "Error: "
            << e.what()
            << '\n';

        return 1;
    }


    return 0;
}
