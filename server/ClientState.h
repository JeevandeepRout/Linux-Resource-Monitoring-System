#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include <string>

#include "../common/SystemData.h"

struct ClientState
{
    std::string clientId;
    std::string hostname;
    std::string ipAddress;

    SystemData latestMetrics;

    std::string status;

    long long lastSeen;

    bool connected;
};

#endif
