#include "AlertManager.h"

#include <iostream>


// ============================================================
// Determine Alert State
// ============================================================

std::string AlertManager::determineAlertState(
    const SystemData& data)
{
    // Critical condition

    if (data.cpuUsage >= 90 ||
        data.memoryUsage >= 90 ||
        data.diskUsage >= 90)
    {
        return "CRITICAL";
    }


    // Warning condition

    if (data.cpuUsage >= 70 ||
        data.memoryUsage >= 70 ||
        data.diskUsage >= 70)
    {
        return "WARNING";
    }


    // Normal condition

    return "NORMAL";
}


// ============================================================
// Print Alert
// ============================================================

void AlertManager::printAlert(
    const SystemData& data,
    const std::string& newState)
{
    if (newState == "CRITICAL")
    {
        std::cout
            << "[CRITICAL] Resource usage is critical on "
            << data.clientId
            << '\n';
    }
    else if (newState == "WARNING")
    {
        std::cout
            << "[WARNING] Resource usage is high on "
            << data.clientId
            << '\n';
    }
    else if (newState == "NORMAL")
    {
        std::cout
            << "[OK] Resource usage returned to normal on "
            << data.clientId
            << '\n';
    }
}


// ============================================================
// Check Alerts
// ============================================================

void AlertManager::checkAlerts(
    const SystemData& data)
{
    // Determine current state

    std::string newState =
        determineAlertState(data);


    // Look for previous state

    auto it =
        clientAlertStates.find(data.clientId);


    // ========================================================
    // First time seeing this client
    // ========================================================

    if (it == clientAlertStates.end())
    {
        clientAlertStates[data.clientId] =
            newState;


        // Only print an alert if the initial state
        // is WARNING or CRITICAL.

        if (newState != "NORMAL")
        {
            printAlert(data, newState);
        }

        return;
    }


    // ========================================================
    // Previous state
    // ========================================================

    std::string previousState =
        it->second;


    // ========================================================
    // State has not changed
    // ========================================================

    if (previousState == newState)
    {
        return;
    }


    // ========================================================
    // State changed
    // ========================================================

    clientAlertStates[data.clientId] =
        newState;

    printAlert(data, newState);
}
