#include "ClientRegistry.h"

// Add or update a client

void ClientRegistry::addOrUpdate(
    const ClientState& client)
{
    clients[client.clientId] = client;
}

// Remove a client

void ClientRegistry::remove(
    const std::string& clientId)
{
    clients.erase(clientId);
}

// Get one client

bool ClientRegistry::get(
    const std::string& clientId,
    ClientState& client)
{
    auto it = clients.find(clientId);

    if (it == clients.end())
    {
        return false;
    }

    client = it->second;

    return true;
}

// Get all clients

std::vector<ClientState> ClientRegistry::getAll()
{
    std::vector<ClientState> result;

    for (const auto& pair : clients)
    {
        result.push_back(pair.second);
    }

    return result;
}
