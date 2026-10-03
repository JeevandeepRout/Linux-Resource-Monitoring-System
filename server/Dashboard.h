#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "ClientRegistry.h"

#include <vector>
#include <atomic>

class Dashboard
{
public:
    void display(
        const std::vector<ClientState>& clients
    );

    void startRefresh(
        ClientRegistry& registry,
        std::atomic<bool>& running
    );
};

#endif
