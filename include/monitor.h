#ifndef MONITOR_H
#define MONITOR_H

#include "system_info.h"
#include "process_info.h"

#include <vector>

struct MonitorData {
    CPUInfo cpuInfo;
    MemoryInfo memoryInfo;
    SystemStats systemStats;
    vector<ProcessInfo> allProcesses;
    vector<ProcessInfo> topMemoryConsumers;
    vector<ProcessInfo> topCPUConsumers;
};

void runOnce();
void runLive(int interval);

MonitorData getMonitorData();
void displayMonitorData(const MonitorData &data);

#endif