#ifndef CLIENT_REGISTRY_H
#define CLIENT_REGISTRY_H

#include <map>
#include <string>
#include <vector>

#include "ClientState.h"

class ClientRegistry
{
public:
    void addOrUpdate(const ClientState& client);

    void remove(const std::string& clientId);

    bool get(
        const std::string& clientId,
        ClientState& client
    );

    std::vector<ClientState> getAll();

private:
    std::map<std::string, ClientState> clients;
};

#endif
