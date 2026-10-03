#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include "Config.h"
#include <string>

class ConfigLoader
{
public:
    static Config load(const std::string& filename);
};

#endif
