#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <mutex>
#include <thread>
#include <string>
#include <vector>

#include "ClientRegistry.h"
#include "AlertManager.h"
#include "Dashboard.h"

class Server
{
public:
    Server(int port);
    ~Server();

    void start();

    void requestShutdown();

    std::vector<ClientState> getClientSnapshot();

private:
    int port;
    int serverSocket;

    ClientRegistry clientRegistry;

    std::mutex dataMutex;

    AlertManager alertManager;
    Dashboard dashboard;

    int offlineTimeout;

    double cpuWarning;
    double cpuCritical;

    double memoryWarning;
    double memoryCritical;

    double diskWarning;
    double diskCritical;

    std::atomic<bool> running;

    void createSocket();
    void bindSocket();
    void listenForClients();
    void acceptClients();

    void handleClient(
        int clientSocket,
        const std::string& clientIp
    );

    void checkOfflineClients();

    std::string determineStatus(
        double cpuUsage,
        double memoryUsage,
        double diskUsage
    );
};

#endif
