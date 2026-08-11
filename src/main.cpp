#include "../include/system_monitor.h"

#include <iomanip>
#include <iostream>

using namespace std;

int main()
{
    try
    {
        // CPU information
        CPUInfo cpu = getCPUInfo();

        // Memory information
        MemoryInfo memory = getMemoryInfo();

        // CPU utilization, uptime and load average
        SystemStats stats = getSystemStats();

        cout << fixed << setprecision(2);

        cout << "==================================================\n";
        cout << "        PROC MONITOR - SYSTEM INFORMATION\n";
        cout << "==================================================\n\n";

        cout << "[1] CPU & MEMORY INFORMATION\n";
        cout << "--------------------------------------------------\n";

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

        cout << "--------------------------------------------------\n\n";


        cout << "[2] CPU UTILIZATION & SYSTEM STATISTICS\n";
        cout << "--------------------------------------------------\n";

        cout << "CPU Usage : "
             << stats.cpuUsage << "%\n";

        cout << "System Uptime : "
             << stats.uptime << '\n';

        cout << "Load Average\n";

        cout << "1 min : "
             << stats.loadAverage.oneMinute << '\n';

        cout << "5 min : "
             << stats.loadAverage.fiveMinutes << '\n';

        cout << "15 min : "
             << stats.loadAverage.fifteenMinutes << '\n';

        cout << "--------------------------------------------------\n";

        return 0;
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
