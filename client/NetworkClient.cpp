#include "NetworkClient.h"

#include <iostream>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


// ============================================================
// CONSTRUCTOR
// ============================================================

NetworkClient::NetworkClient()
    : socketFd(-1),
      connected(false)
{
}


// ============================================================
// CONNECT TO SERVER
// ============================================================

bool NetworkClient::connectToServer(
    const std::string& host,
    int port
)
{
    // If an old socket exists, close it first
    if (socketFd != -1)
    {
        close(socketFd);
        socketFd = -1;
    }

    connected = false;


    // Create socket
    socketFd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );


    if (socketFd < 0)
    {
        std::cerr
            << "Failed to create client socket.\n";

        return false;
    }


    sockaddr_in serverAddress{};

    serverAddress.sin_family =
        AF_INET;

    serverAddress.sin_port =
        htons(port);


    // Convert IP address
    if (inet_pton(
        AF_INET,
        host.c_str(),
        &serverAddress.sin_addr
    ) <= 0)
    {
        std::cerr
            << "Invalid server address: "
            << host
            << "\n";

        close(socketFd);

        socketFd = -1;

        return false;
    }


    // Connect
    if (connect(
        socketFd,
        (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)
    ) < 0)
    {
        close(socketFd);

        socketFd = -1;

        connected = false;

        return false;
    }


    connected = true;

    std::cout
        << "Connected to server "
        << host
        << ":"
        << port
        << "\n";

    return true;
}


// ============================================================
// SEND MESSAGE
// ============================================================

bool NetworkClient::sendMessage(
    const std::string& message
)
{
    if (!connected || socketFd == -1)
    {
        return false;
    }


    ssize_t bytesSent = send(
        socketFd,
        message.c_str(),
        message.size(),
        0
    );


    if (bytesSent < 0)
    {
        std::cerr
            << "Failed to send message.\n";

        connected = false;

        close(socketFd);

        socketFd = -1;

        return false;
    }


    return true;
}


// ============================================================
// DISCONNECT
// ============================================================

void NetworkClient::disconnect()
{
    if (socketFd != -1)
    {
        close(socketFd);

        socketFd = -1;
    }

    connected = false;
}


// ============================================================
// CHECK CONNECTION
// ============================================================

bool NetworkClient::isConnected() const
{
    return connected;
}
