#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <string>

struct CPUInfo{
    std::string modelName;
    int logicalProcessors;
};

struct MemoryInfo{
    double totalGB;
    double freeGB;
    double availableGB;
    double usagePercentage;
};

struct CPUTime{
    unsigned long long total;
    unsigned long long idle;
};

struct LoadAverage{
    double oneMinute;
    double fiveMinutes;
    double fifteenMinutes;
};

struct SystemStats{
    double cpuUsage;
    std::string uptime;
    LoadAverage loadAverage;
};

CPUInfo getCPUInfo();

MemoryInfo getMemoryInfo();

CPUTime getCPUTime();

double calculateCPUUsage(
    const CPUTime& previous,
    const CPUTime& current
);

std::string getUptime();

LoadAverage getLoadAverage();

SystemStats getSystemStats();

#endif
