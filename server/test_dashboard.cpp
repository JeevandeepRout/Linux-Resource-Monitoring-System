#include "Dashboard.h"

int main()
{
    Dashboard dashboard;

    std::vector<ClientState> clients;

    ClientState client;

    client.clientId = "PC-945742";
    client.hostname = "Ubuntu";
    client.ipAddress = "127.0.0.1";

    client.latestMetrics.clientId = "PC-945742";
    client.latestMetrics.hostname = "Ubuntu";
    client.latestMetrics.kernelVersion = "7.0.0";

    client.latestMetrics.cpuUsage = 1.8;
    client.latestMetrics.memoryUsage = 74.1;
    client.latestMetrics.diskUsage = 31.8;
    client.latestMetrics.processCount = 314;
    client.latestMetrics.uptimeSeconds = 33776.2;

    client.status = "WARNING";
    client.lastSeen = 1790943339;
    client.connected = true;

    clients.push_back(client);

    dashboard.display(clients);

    return 0;
}
