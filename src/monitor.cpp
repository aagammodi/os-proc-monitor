#include "../include/monitor.h"
#include "../include/system_info.h"
#include "../include/process_info.h"

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <algorithm>

using namespace std;

int WIDTH = 60;

void runOnce(){
    cout << fixed << setprecision(2);

    // Get all the data to diplay
    MonitorData data = getMonitorData();
    displayMonitorData(data);
}

void runLive(int interval){
    cout << fixed << setprecision(2);
    system("clear");

    while(true){
        // Getting the data before the "clear" command to avoid displaying the partial data
        MonitorData data = getMonitorData();

        // Clear terminal
        system("clear");
        
        displayMonitorData(data);

        // Wait till next refresh
        sleep(interval);
    }
}

MonitorData getMonitorData(){
    MonitorData data;

    data.cpuInfo = getCPUInfo();
    data.memoryInfo = getMemoryInfo();
    data.systemStats = getSystemStats();
    data.allProcesses = getProcessInfo();
    vector<ProcessInfo> allProcesses = data.allProcesses;

    // Sort processes by memory usage
    sort(allProcesses.begin(), allProcesses.end(), 
        [](const ProcessInfo &a, const ProcessInfo &b) {
            return a.vmRSSMB > b.vmRSSMB;
    });

    // Get top 5 memory consumers
    for (int i = 0; i < 5 && i < allProcesses.size(); ++i)
    {
        data.topMemoryConsumers.push_back(allProcesses[i]);
    }

    // Sampling the processes over a 1-second interval to calculate CPU usage
    sampleProcesses(allProcesses, data.cpuInfo.logicalProcessors);

    // Sort processes by CPU usage
    sort(allProcesses.begin(), allProcesses.end(),
        [](const ProcessInfo &a, const ProcessInfo &b) {
            return a.cpuUsage > b.cpuUsage;
    });

    // Get top 5 CPU consumers
    for (int i = 0; i < 5 && i < allProcesses.size(); ++i)
    {
        data.topCPUConsumers.push_back(allProcesses[i]);
    }

    return data;
}

void displayMonitorData(const MonitorData &data){
    // Title
    cout << string(WIDTH, '=') << "\n";
    cout << "        PROC MONITOR - SYSTEM INFORMATION\n";
    cout << string(WIDTH, '=') << "\n";

    
    // Display CPU and Memory Information
    cout << "[1] CPU & MEMORY INFORMATION\n";
    cout << string(WIDTH, '-') << "\n";

    cout << "CPU Model : "
            << data.cpuInfo.modelName << '\n';

    cout << "Logical CPUs : "
            << data.cpuInfo.logicalProcessors << '\n';

    cout << "Total Memory : "
            << data.memoryInfo.totalGB << " GB\n";

    cout << "Available Memory : "
            << data.memoryInfo.availableGB << " GB\n";

    cout << "Free Memory : "
            << data.memoryInfo.freeGB << " GB\n";

    cout << "Memory Usage : "
            << data.memoryInfo.usagePercentage << "%\n";

    cout << string(WIDTH, '-') << "\n";


    // Display CPU Utilization and System Statistics
    cout << "[2] CPU UTILIZATION & SYSTEM STATISTICS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << "CPU Usage : "
            << data.systemStats.cpuUsage << "%\n";

    cout << "System Uptime : "
            << data.systemStats.uptime << '\n';

    cout << "Load Average\n";

    cout << " 1 min : "
            << data.systemStats.loadAverage.oneMinute << '\n';

    cout << " 5 min : "
            << data.systemStats.loadAverage.fiveMinutes << '\n';

    cout << "15 min : "
            << data.systemStats.loadAverage.fifteenMinutes << '\n';

    cout << string(WIDTH, '-') << "\n";


    // Display Running Processes
    std::cout << "[3] RUNNING PROCESSES\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left
         << setw(8) << "PID"
         << setw(8) << "PPID"
         << setw(10) << "State"
         << setw(10) << "Threads"
         << setw(12) << "VmSize(MB)"
         << setw(42) << "Process Name"
         << "\n";

    for (const auto &p : data.allProcesses)
    {
        cout << left
             << setw(8) << p.pid
             << setw(8) << p.ppid
             << setw(10) << p.state
             << setw(10) << p.threads
             << setw(12) << p.vmSizeMB
             << setw(42) << p.name
             << "\n";
    }
    cout << string(WIDTH, '-') << "\n";


    // Display Top 5 Memory Consumers & Top 5 CPU Consumers
    std::cout << "[4] TOP 5 MEMORY CONSUMERS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left
        << setw(8) << "Rank"
        << setw(8) << "PID"
        << setw(12) << "Memory(MB)"
        << setw(42) << "Process Name"
        << "\n";

    for (size_t i = 0; i < data.topMemoryConsumers.size(); ++i)
    {
        const auto &p = data.topMemoryConsumers[i];
        cout << left
             << setw(8) << (i + 1)
             << setw(8) << p.pid
             << setw(12) << p.vmRSSMB
             << setw(42) << p.name
             << "\n";
    }
    cout << string(WIDTH, '-') << "\n";


    std::cout << "TOP 5 CPU CONSUMERS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left
        << setw(8) << "Rank"
        << setw(8) << "PID"
        << setw(12) << "CPU Usage(%)"
        << setw(42) << "Process Name"
        << "\n";

    for (size_t i = 0; i < data.topCPUConsumers.size(); ++i)
    {
        const auto &p = data.topCPUConsumers[i];
        cout << left
             << setw(8) << (i + 1)
             << setw(8) << p.pid
             << setw(12) << p.cpuUsage
             << setw(42) << p.name
             << "\n";
    }
    cout << string(WIDTH, '=') << "\n";
}