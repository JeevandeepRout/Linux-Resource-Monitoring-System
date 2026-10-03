#ifndef CLIENT_IDENTITY_H
#define CLIENT_IDENTITY_H

#include <string>

class ClientIdentity
{
public:
    std::string getClientId();

private:
    std::string loadOrCreateId();
};

#endif
