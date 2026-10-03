#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include "../common/SystemData.h"

#include <map>
#include <string>

class AlertManager
{
public:
    void checkAlerts(const SystemData& data);

private:
    std::map<std::string, std::string> clientAlertStates;

    std::string determineAlertState(
        const SystemData& data
    );

    void printAlert(
        const SystemData& data,
        const std::string& newState
    );
};

#endif
