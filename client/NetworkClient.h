#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <string>

class NetworkClient
{
public:

    NetworkClient();

    bool connectToServer(
        const std::string& host,
        int port
    );

    bool sendMessage(
        const std::string& message
    );

    void disconnect();

    bool isConnected() const;

private:

    int socketFd;
    bool connected;
};

#endif
