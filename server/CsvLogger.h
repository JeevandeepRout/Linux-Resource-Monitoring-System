#ifndef CSV_LOGGER_H
#define CSV_LOGGER_H

#include "../common/SystemData.h"

class CsvLogger
{
public:
    void logMetrics(const SystemData& data);

private:
    void createHeaderIfNeeded();
};

#endif
