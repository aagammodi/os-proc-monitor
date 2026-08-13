#include "../include/monitor.h"
#include "../include/system_info.h"
#include "../include/process_info.h"

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <algorithm>

using namespace std;

constexpr int WIDTH = 90;

// Gets all the data and displays it once
void runOnce(){
    cout << fixed << setprecision(2);

    // Get all the data to diplay
    MonitorData data = getMonitorData();
    displayMonitorData(data);
}

// Continuously gets all the data and displays it every "interval" seconds
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

// Gets all the data and returns it in a MonitorData struct
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
            return a.vmRSSKB > b.vmRSSKB;
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

// Displays the data in a formatted manner
void displayMonitorData(const MonitorData &data){
    // Title
    cout << string(WIDTH, '=') << "\n";
    const string title = "PROC MONITOR - SYSTEM INFORMATION";
    cout << string((WIDTH - title.size()) / 2, ' ') << title << "\n";
    cout << string(WIDTH, '=') << "\n";

    
    // Display CPU and Memory Information
    cout << "[1] CPU & MEMORY INFORMATION\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left << setw(22) << "CPU Model" << ": ";
    cout << data.cpuInfo.modelName << '\n';

    cout << left << setw(22) << "Logical CPUs" << ": ";
    cout << data.cpuInfo.logicalProcessors << '\n';

    cout << left << setw(22) << "Total Memory" << ": ";
    cout << data.memoryInfo.totalGB << " GB\n";

    cout << left << setw(22) << "Available Memory" << ": ";
    cout << data.memoryInfo.availableGB << " GB\n";

    cout << left << setw(22) << "Free Memory" << ": ";
    cout << data.memoryInfo.freeGB << " GB\n";

    cout << left << setw(22) << "Memory Usage" << ": ";
    cout << data.memoryInfo.usagePercentage << "%\n";

    cout << string(WIDTH, '-') << "\n";


    // Display CPU Utilization and System Statistics
    cout << "[2] CPU UTILIZATION & SYSTEM STATISTICS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left << setw(22) << "CPU Usage" << ": ";
    cout << data.systemStats.cpuUsage << "%\n";

    cout << left << setw(22) << "System Uptime" << ": ";
    cout << data.systemStats.uptime << '\n';

    cout << "Load Average\n";

    cout << "  1 min : ";
    cout << data.systemStats.loadAverage.oneMinute << '\n';

    cout << "  5 min : ";
    cout << data.systemStats.loadAverage.fiveMinutes << '\n';

    cout << " 15 min : ";
    cout << data.systemStats.loadAverage.fifteenMinutes << '\n';

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
             << setw(12) << p.vmSizeKB / 1024
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
        << setw(14) << "Memory(MB)"
        << setw(42) << "Process Name"
        << "\n";

    for (size_t i = 0; i < data.topMemoryConsumers.size(); ++i)
    {
        const auto &p = data.topMemoryConsumers[i];
        cout << left
             << setw(8) << (i + 1)
             << setw(8) << p.pid
             << setw(14) << p.vmRSSKB / 1024
             << setw(42) << p.name
             << "\n";
    }
    cout << string(WIDTH, '-') << "\n";


    std::cout << "TOP 5 CPU CONSUMERS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << left
        << setw(8) << "Rank"
        << setw(8) << "PID"
        << setw(14) << "CPU Usage(%)"
        << setw(42) << "Process Name"
        << "\n";

    for (size_t i = 0; i < data.topCPUConsumers.size(); ++i)
    {
        const auto &p = data.topCPUConsumers[i];
        cout << left
             << setw(8) << (i + 1)
             << setw(8) << p.pid
             << setw(14) << p.cpuUsage
             << setw(42) << p.name
             << "\n";
    }
    cout << string(WIDTH, '=') << "\n";
}
