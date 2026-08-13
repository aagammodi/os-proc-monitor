#ifndef MONITOR_H
#define MONITOR_H

#include "system_info.h"
#include "process_info.h"

#include <vector>

struct MonitorData
{
    CPUInfo cpuInfo;
    MemoryInfo memoryInfo;
    SystemStats systemStats;

    std::vector<ProcessInfo> allProcesses;
    std::vector<ProcessInfo> topMemoryConsumers;
    std::vector<ProcessInfo> topCPUConsumers;
};

// Static mode
void runOnce();

// Live mode
void runLive(int interval);

void runScrollableMonitor(int interval);

// Collect all monitoring data
MonitorData getMonitorData();

// Print one snapshot (used by static mode)
void displayMonitorData(const MonitorData &data);

#endif
