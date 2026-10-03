#include "Logger.h"

int main()
{
    Logger::info("Server started.");
    Logger::warning("Test warning.");
    Logger::error("Test error.");

    return 0;
}
