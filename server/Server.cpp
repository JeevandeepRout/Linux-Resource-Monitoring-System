#include "Server.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <csignal>
#include <cerrno>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <fcntl.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


// ============================================================
// GLOBAL SHUTDOWN FLAG
// ============================================================

volatile sig_atomic_t shutdownRequested = 0;


// ============================================================
// SIGNAL HANDLER
// Ctrl + C sends SIGINT
// ============================================================

void handleSignal(int signal)
{
    if (signal == SIGINT)
    {
        shutdownRequested = 1;
    }
}


// ============================================================
// CONSTRUCTOR
// ============================================================

Server::Server(int port)
    : port(port),
      serverSocket(-1),
      offlineTimeout(15),

      cpuWarning(70),
      cpuCritical(90),

      memoryWarning(70),
      memoryCritical(90),

      diskWarning(70),
      diskCritical(90),

      running(true)
{
}


// ============================================================
// DESTRUCTOR
// ============================================================

Server::~Server()
{
    if (serverSocket != -1)
    {
        close(serverSocket);
        serverSocket = -1;
    }
}


// ============================================================
// CREATE SOCKET
// ============================================================

void Server::createSocket()
{
    serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (serverSocket < 0)
    {
        std::cerr
            << "Socket creation failed.\n";

        exit(EXIT_FAILURE);
    }

    std::cout
        << "Socket created successfully.\n";


    // Allow port reuse
    int option = 1;

    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &option,
        sizeof(option)
    );


    // ========================================================
    // MAKE LISTENING SOCKET NON-BLOCKING
    // ========================================================

    int flags = fcntl(
        serverSocket,
        F_GETFL,
        0
    );

    if (flags < 0)
    {
        std::cerr
            << "Failed to get socket flags.\n";

        close(serverSocket);
        serverSocket = -1;

        exit(EXIT_FAILURE);
    }


    if (fcntl(
        serverSocket,
        F_SETFL,
        flags | O_NONBLOCK
    ) < 0)
    {
        std::cerr
            << "Failed to make socket non-blocking.\n";

        close(serverSocket);
        serverSocket = -1;

        exit(EXIT_FAILURE);
    }
}


// ============================================================
// BIND SOCKET
// ============================================================

void Server::bindSocket()
{
    sockaddr_in serverAddress{};

    serverAddress.sin_family =
        AF_INET;

    serverAddress.sin_addr.s_addr =
        INADDR_ANY;

    serverAddress.sin_port =
        htons(port);


    if (bind(
        serverSocket,
        (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)
    ) < 0)
    {
        std::cerr
            << "Bind failed.\n";

        close(serverSocket);
        serverSocket = -1;

        exit(EXIT_FAILURE);
    }

    std::cout
        << "Socket bound to port "
        << port
        << ".\n";
}


// ============================================================
// LISTEN
// ============================================================

void Server::listenForClients()
{
    if (listen(
        serverSocket,
        10
    ) < 0)
    {
        std::cerr
            << "Listen failed.\n";

        close(serverSocket);
        serverSocket = -1;

        exit(EXIT_FAILURE);
    }

    std::cout
        << "Server is listening...\n";
}


// ============================================================
// DETERMINE STATUS
// ============================================================

std::string Server::determineStatus(
    double cpuUsage,
    double memoryUsage,
    double diskUsage
)
{
    if (
        cpuUsage >= cpuCritical ||
        memoryUsage >= memoryCritical ||
        diskUsage >= diskCritical
    )
    {
        return "CRITICAL";
    }

    if (
        cpuUsage >= cpuWarning ||
        memoryUsage >= memoryWarning ||
        diskUsage >= diskWarning
    )
    {
        return "WARNING";
    }

    return "NORMAL";
}


// ============================================================
// ACCEPT CLIENTS
// ============================================================

void Server::acceptClients()
{
    while (running)
    {
        // ====================================================
        // CHECK CTRL+C
        // ====================================================

        if (shutdownRequested)
        {
            requestShutdown();
            break;
        }


        sockaddr_in clientAddress{};

        socklen_t clientLength =
            sizeof(clientAddress);


        int clientSocket = accept(
            serverSocket,
            (struct sockaddr*)&clientAddress,
            &clientLength
        );


        // ====================================================
        // NO CONNECTION AVAILABLE
        // ====================================================

        if (clientSocket < 0)
        {
            if (
                errno == EAGAIN ||
                errno == EWOULDBLOCK
            )
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100)
                );

                continue;
            }


            // Server is shutting down
            if (!running)
            {
                break;
            }


            std::cerr
                << "Accept failed.\n";

            continue;
        }


        // ====================================================
        // GET CLIENT IP
        // ====================================================

        char clientIp[INET_ADDRSTRLEN];

        inet_ntop(
            AF_INET,
            &clientAddress.sin_addr,
            clientIp,
            INET_ADDRSTRLEN
        );


        std::string ipAddress(
            clientIp
        );


        std::cout
            << "\nNew client connected from "
            << ipAddress
            << ".\n";


        // ====================================================
        // CREATE CLIENT THREAD
        // ====================================================

        std::thread(
            &Server::handleClient,
            this,
            clientSocket,
            ipAddress
        ).detach();
    }


    std::cout
        << "Stopped accepting new clients.\n";
}


// ============================================================
// HANDLE CLIENT
// ============================================================

void Server::handleClient(
    int clientSocket,
    const std::string& clientIp
)
{
    std::cout
        << "Client handler started for "
        << clientIp
        << ".\n";


    std::string receiveBuffer;

    char buffer[4096];


    while (running)
    {
        memset(
            buffer,
            0,
            sizeof(buffer)
        );


        ssize_t bytesReceived =
            recv(
                clientSocket,
                buffer,
                sizeof(buffer) - 1,
                0
            );


        // ====================================================
        // CLIENT DISCONNECTED
        // ====================================================

        if (bytesReceived == 0)
        {
            break;
        }


        // ====================================================
        // RECEIVE ERROR
        // ====================================================

        if (bytesReceived < 0)
        {
            if (running)
            {
                std::cerr
                    << "Receive failed for client "
                    << clientIp
                    << ".\n";
            }

            break;
        }


        // ====================================================
        // ADD RECEIVED DATA TO BUFFER
        // ====================================================

        receiveBuffer.append(
            buffer,
            bytesReceived
        );


        // ====================================================
        // TCP FRAMING
        // Messages are separated by '\n'
        // ====================================================

        size_t newlinePosition;


        while (
            (newlinePosition =
                receiveBuffer.find('\n'))
            != std::string::npos
        )
        {
            std::string message =
                receiveBuffer.substr(
                    0,
                    newlinePosition
                );


            // Remove processed message
            receiveBuffer.erase(
                0,
                newlinePosition + 1
            );


            // Ignore empty messages
            if (message.empty())
            {
                continue;
            }


            try
            {
                // ============================================
                // PARSE JSON
                // ============================================

                json data =
                    json::parse(message);


                // ============================================
                // PRETTY PRINT JSON
                // ============================================

                std::cout
                    << "\n";

                std::cout
                    << "Received JSON:\n";

                std::cout
                    << data.dump(4)
                    << "\n";


                // ============================================
                // REQUIRED FIELDS
                // ============================================

                const std::vector<std::string>
                    requiredFields =
                {
                    "type",
                    "client_id",
                    "hostname",
                    "kernel_version",
                    "cpu_usage",
                    "memory_usage",
                    "disk_usage",
                    "process_count",
                    "uptime_seconds",
                    "timestamp"
                };


                bool valid = true;


                for (
                    const auto& field :
                    requiredFields
                )
                {
                    if (!data.contains(field))
                    {
                        std::cerr
                            << "Missing required field: "
                            << field
                            << "\n";

                        valid = false;
                    }
                }


                if (!valid)
                {
                    std::cerr
                        << "Invalid message rejected.\n";

                    continue;
                }


                // ============================================
                // CHECK MESSAGE TYPE
                // ============================================

                if (
                    !data["type"].is_string() ||
                    data["type"] != "metrics"
                )
                {
                    std::cerr
                        << "Invalid message type.\n";

                    continue;
                }


                // ============================================
                // EXTRACT VALUES
                // ============================================

                std::string clientId =
                    data["client_id"];

                std::string hostname =
                    data["hostname"];

                std::string kernelVersion =
                    data["kernel_version"];

                double cpuUsage =
                    data["cpu_usage"];

                double memoryUsage =
                    data["memory_usage"];

                double diskUsage =
                    data["disk_usage"];

                int processCount =
                    data["process_count"];

                double uptimeSeconds =
                    data["uptime_seconds"];

                long long timestamp =
                    data["timestamp"];


                // ============================================
                // RANGE VALIDATION
                // ============================================

                if (
                    cpuUsage < 0 ||
                    cpuUsage > 100
                )
                {
                    std::cerr
                        << "Invalid CPU usage.\n";

                    continue;
                }


                if (
                    memoryUsage < 0 ||
                    memoryUsage > 100
                )
                {
                    std::cerr
                        << "Invalid memory usage.\n";

                    continue;
                }


                if (
                    diskUsage < 0 ||
                    diskUsage > 100
                )
                {
                    std::cerr
                        << "Invalid disk usage.\n";

                    continue;
                }


                if (processCount < 0)
                {
                    std::cerr
                        << "Invalid process count.\n";

                    continue;
                }


                if (uptimeSeconds < 0)
                {
                    std::cerr
                        << "Invalid uptime.\n";

                    continue;
                }


                // ============================================
                // CREATE SYSTEM DATA
                // ============================================

                SystemData metrics;

                metrics.clientId =
                    clientId;

                metrics.hostname =
                    hostname;

                metrics.kernelVersion =
                    kernelVersion;

                metrics.cpuUsage =
                    cpuUsage;

                metrics.memoryUsage =
                    memoryUsage;

                metrics.diskUsage =
                    diskUsage;

                metrics.processCount =
                    processCount;

                metrics.uptimeSeconds =
                    uptimeSeconds;

                metrics.timestamp =
                    timestamp;


                // ============================================
                // DETERMINE STATUS
                // ============================================

                std::string status =
                    determineStatus(
                        cpuUsage,
                        memoryUsage,
                        diskUsage
                    );


                // ============================================
                // ALERT MANAGER
                // ============================================

                alertManager.checkAlerts(
                    metrics
                );


                // ============================================
                // CREATE CLIENT STATE
                // ============================================

                ClientState client;

                client.clientId =
                    clientId;

                client.hostname =
                    hostname;

                client.ipAddress =
                    clientIp;

                client.latestMetrics =
                    metrics;

                client.status =
                    status;

                client.lastSeen =
                    std::chrono::duration_cast<
                        std::chrono::seconds
                    >(
                        std::chrono::system_clock::now()
                        .time_since_epoch()
                    ).count();

                client.connected =
                    true;


                // ============================================
                // UPDATE CLIENT REGISTRY
                // ============================================

                {
                    std::lock_guard<std::mutex>
                        lock(dataMutex);

                    clientRegistry.addOrUpdate(
                        client
                    );
                }


                // ============================================
                // PRINT CLIENT INFORMATION
                // ============================================

                std::cout
                    << "Client: "
                    << clientId
                    << "\n";

                std::cout
                    << "Status: "
                    << status
                    << "\n";

                std::cout
                    << "Last seen: "
                    << client.lastSeen
                    << "\n";


                // ============================================
                // CREATE ACK
                // ============================================

                json ack;

                ack["type"] =
                    "ack";

                ack["status"] =
                    "accepted";

                ack["timestamp"] =
                    std::chrono::duration_cast<
                        std::chrono::seconds
                    >(
                        std::chrono::system_clock::now()
                        .time_since_epoch()
                    ).count();


                std::string ackMessage =
                    ack.dump() + "\n";


                // ============================================
                // SEND ACK
                // ============================================

                send(
                    clientSocket,
                    ackMessage.c_str(),
                    ackMessage.size(),
                    0
                );


                std::cout
                    << "ACK sent to client.\n";
            }
            catch (
                const json::exception& e
            )
            {
                std::cerr
                    << "Invalid JSON received: "
                    << e.what()
                    << "\n";
            }
            catch (
                const std::exception& e
            )
            {
                std::cerr
                    << "Error processing client data: "
                    << e.what()
                    << "\n";
            }
        }
    }


    // ========================================================
    // CLOSE CLIENT SOCKET
    // ========================================================

    close(clientSocket);


    std::cout
        << "Client disconnected: "
        << clientIp
        << ".\n";
}


// ============================================================
// OFFLINE CLIENT CHECK
// ============================================================

void Server::checkOfflineClients()
{
    while (running)
    {
        // Check every 5 seconds
        std::this_thread::sleep_for(
            std::chrono::seconds(5)
        );


        if (!running)
        {
            break;
        }


        long long currentTime =
            std::chrono::duration_cast<
                std::chrono::seconds
            >(
                std::chrono::system_clock::now()
                .time_since_epoch()
            ).count();


        std::vector<ClientState> clients;


        // ====================================================
        // GET PROTECTED COPY OF CLIENTS
        // ====================================================

        {
            std::lock_guard<std::mutex>
                lock(dataMutex);

            clients =
                clientRegistry.getAll();
        }


        // ====================================================
        // CHECK EACH CLIENT
        // ====================================================

        for (
            const auto& client :
            clients
        )
        {
            long long elapsed =
                currentTime -
                client.lastSeen;


            if (
                elapsed > offlineTimeout &&
                client.status != "OFFLINE"
            )
            {
                ClientState updatedClient =
                    client;


                updatedClient.status =
                    "OFFLINE";

                updatedClient.connected =
                    false;


                // ============================================
                // UPDATE REGISTRY
                // ============================================

                {
                    std::lock_guard<std::mutex>
                        lock(dataMutex);

                    clientRegistry.addOrUpdate(
                        updatedClient
                    );
                }


                std::cout
                    << "Client "
                    << client.clientId
                    << " is OFFLINE.\n";
            }
        }
    }
}


// ============================================================
// GET CLIENT SNAPSHOT
// ============================================================

std::vector<ClientState>
Server::getClientSnapshot()
{
    std::lock_guard<std::mutex>
        lock(dataMutex);

    return clientRegistry.getAll();
}


// ============================================================
// GRACEFUL SHUTDOWN
// ============================================================

void Server::requestShutdown()
{
    if (!running)
    {
        return;
    }


    std::cout
        << "\nStopping server...\n";


    // Stop all server loops
    running = false;


    // Close listening socket
    if (serverSocket != -1)
    {
        close(serverSocket);

        serverSocket = -1;
    }
}


// ============================================================
// START SERVER
// ============================================================

void Server::start()
{
    // ========================================================
    // CREATE SOCKET
    // ========================================================

    createSocket();


    // ========================================================
    // BIND SOCKET
    // ========================================================

    bindSocket();


    // ========================================================
    // LISTEN
    // ========================================================

    listenForClients();


    // ========================================================
    // REGISTER CTRL+C HANDLER
    // ========================================================

    std::signal(
        SIGINT,
        handleSignal
    );


    // ========================================================
    // START DASHBOARD
    // ========================================================

    std::thread([this]()
    {
        dashboard.startRefresh(
            clientRegistry,
            running
        );
    }).detach();


    // ========================================================
    // START OFFLINE CLIENT CHECKER
    // ========================================================

    std::thread(
        &Server::checkOfflineClients,
        this
    ).detach();


    // ========================================================
    // ACCEPT CLIENTS
    // ========================================================

    acceptClients();


    // ========================================================
    // SERVER SHUTDOWN COMPLETE
    // ========================================================

    std::cout
        << "Server shutdown complete.\n";
}
