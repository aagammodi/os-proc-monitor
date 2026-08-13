#include "../include/monitor.h"
#include "../include/system_info.h"
#include "../include/process_info.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

constexpr int WIDTH = 90;

void runOnce()
{
    cout << fixed << setprecision(2);

    MonitorData data = getMonitorData();
    displayMonitorData(data);
}

void runLive(int interval)
{
    cout << fixed << setprecision(2);
    runScrollableMonitor(interval);
}

MonitorData getMonitorData()
{
    MonitorData data;

    data.cpuInfo = getCPUInfo();
    data.memoryInfo = getMemoryInfo();
    data.systemStats = getSystemStats();
    data.allProcesses = getProcessInfo();

    vector<ProcessInfo> allProcesses = data.allProcesses;

    sort(allProcesses.begin(), allProcesses.end(),
        [](const ProcessInfo &a, const ProcessInfo &b) {
            return a.vmRSSKB > b.vmRSSKB;
        });

    for(int i = 0; i < 5 && i < (int)allProcesses.size(); ++i)
        data.topMemoryConsumers.push_back(allProcesses[i]);

    sampleProcesses(allProcesses, data.cpuInfo.logicalProcessors);

    sort(allProcesses.begin(), allProcesses.end(),
        [](const ProcessInfo &a, const ProcessInfo &b) {
            return a.cpuUsage > b.cpuUsage;
        });

    for(int i = 0; i < 5 && i < (int)allProcesses.size(); ++i)
        data.topCPUConsumers.push_back(allProcesses[i]);

    return data;
}

void displayMonitorData(const MonitorData &data)
{
    cout << string(WIDTH, '=') << "\n";

    const string title = "PROC MONITOR - SYSTEM INFORMATION";
    cout << string((WIDTH - title.size()) / 2, ' ') << title << "\n";

    cout << string(WIDTH, '=') << "\n";

    cout << "[1] CPU & MEMORY INFORMATION\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left << setw(22) << "CPU Model" << ": " << data.cpuInfo.modelName << '\n';
    cout << left << setw(22) << "Logical CPUs" << ": " << data.cpuInfo.logicalProcessors << '\n';
    cout << left << setw(22) << "Total Memory" << ": " << data.memoryInfo.totalGB << " GB\n";
    cout << left << setw(22) << "Available Memory" << ": " << data.memoryInfo.availableGB << " GB\n";
    cout << left << setw(22) << "Free Memory" << ": " << data.memoryInfo.freeGB << " GB\n";
    cout << left << setw(22) << "Memory Usage" << ": " << data.memoryInfo.usagePercentage << "%\n";

    cout << string(WIDTH, '-') << "\n";

    cout << "[2] CPU UTILIZATION & SYSTEM STATISTICS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left << setw(22) << "CPU Usage" << ": " << data.systemStats.cpuUsage << "%\n";
    cout << left << setw(22) << "System Uptime" << ": " << data.systemStats.uptime << '\n';

    cout << "Load Average\n";
    cout << "  1 min : " << data.systemStats.loadAverage.oneMinute << '\n';
    cout << "  5 min : " << data.systemStats.loadAverage.fiveMinutes << '\n';
    cout << " 15 min : " << data.systemStats.loadAverage.fifteenMinutes << '\n';

    cout << string(WIDTH, '-') << "\n";

    cout << "[3] RUNNING PROCESSES\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left
         << setw(8) << "PID"
         << setw(8) << "PPID"
         << setw(10) << "State"
         << setw(10) << "Threads"
         << setw(12) << "VmSize(MB)"
         << setw(42) << "Process Name"
         << "\n";

    for(const auto &p : data.allProcesses)
    {
        cout << left
             << setw(8) << p.pid
             << setw(8) << p.ppid
             << setw(10) << p.state
             << setw(10) << p.threads
             << setw(12) << p.vmSizeKB / 1024
             << setw(42) << p.name
             << "\n";
    }

    cout << string(WIDTH, '-') << "\n";

    cout << "[4] TOP 5 MEMORY CONSUMERS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left
         << setw(8) << "Rank"
         << setw(8) << "PID"
         << setw(14) << "Memory(MB)"
         << setw(42) << "Process Name"
         << "\n";

    for(size_t i = 0; i < data.topMemoryConsumers.size(); ++i)
    {
        const auto &p = data.topMemoryConsumers[i];

        cout << left
             << setw(8) << i + 1
             << setw(8) << p.pid
             << setw(14) << p.vmRSSKB / 1024
             << setw(42) << p.name
             << "\n";
    }

    cout << string(WIDTH, '-') << "\n";

    cout << "TOP 5 CPU CONSUMERS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left
         << setw(8) << "Rank"
         << setw(8) << "PID"
         << setw(14) << "CPU Usage(%)"
         << setw(42) << "Process Name"
         << "\n";

    for(size_t i = 0; i < data.topCPUConsumers.size(); ++i)
    {
        const auto &p = data.topCPUConsumers[i];

        cout << left
             << setw(8) << i + 1
             << setw(8) << p.pid
             << setw(14) << p.cpuUsage
             << setw(42) << p.name
             << "\n";
    }

    cout << string(WIDTH, '=') << "\n";
}
