#include "../include/monitor.h"
#include "../include/system_info.h"

#include<iostream>
#include<iomanip>
#include<unistd.h>


using namespace std;

int WIDTH = 50;

void runOnce(){
    cout << fixed << setprecision(2);

    cout << string(WIDTH, '=') << "\n";
    cout << "        PROC MONITOR - SYSTEM INFORMATION\n";
    cout << string(WIDTH, '=') << "\n";

    displaySystemInfo();
    displayProcesses();
    displayTopProcesses();
}

void runLive(int interval){
    while(true){
        //clear terminal
        system("clear");

        runOnce();

        //wait till next refresh
        sleep(interval);
    }
}

void displaySystemInfo(){
    //get data
    CPUInfo cpu = getCPUInfo();
    MemoryInfo memory = getMemoryInfo();
    SystemStats stats = getSystemStats();

    cout << "[1] CPU & MEMORY INFORMATION\n";
    cout << string(WIDTH, '-') << "\n";

    cout << "CPU Model : "
            << cpu.modelName << '\n';

    cout << "Logical CPUs : "
            << cpu.logicalProcessors << '\n';

    cout << "Total Memory : "
            << memory.totalGB << " GB\n";

    cout << "Available Memory : "
            << memory.availableGB << " GB\n";

    cout << "Free Memory : "
            << memory.freeGB << " GB\n";

    cout << "Memory Usage : "
            << memory.usagePercentage << "%\n";

    cout << string(WIDTH, '-') << "\n";


    cout << "[2] CPU UTILIZATION & SYSTEM STATISTICS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << "CPU Usage : "
            << stats.cpuUsage << "%\n";

    cout << "System Uptime : "
            << stats.uptime << '\n';

    cout << "Load Average\n";

    cout << " 1 min : "
            << stats.loadAverage.oneMinute << '\n';

    cout << " 5 min : "
            << stats.loadAverage.fiveMinutes << '\n';

    cout << "15 min : "
            << stats.loadAverage.fifteenMinutes << '\n';

    cout << string(WIDTH, '-') << "\n";
}

void displayProcesses()
{
    std::cout << "\n[3] RUNNING PROCESSES\n";
    cout << string(WIDTH, '-') << "\n";

    cout << "\n\n\n\n";

    cout << string(WIDTH, '-') << "\n";
}

void displayTopProcesses()
{
    std::cout << "\n[4] TOP 5 MEMORY CONSUMERS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << "\n\n\n\n";


    cout << string(WIDTH, '-') << "\n";

    std::cout << "\nTOP 5 CPU CONSUMERS\n";
    cout << string(WIDTH, '-') << "\n";

    cout << "\n\n\n\n";


    cout << string(WIDTH, '=') << "\n";
}