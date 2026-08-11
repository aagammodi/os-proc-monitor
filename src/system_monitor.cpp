#include "../include/system_monitor.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <chrono>

using namespace std;


// ------------------------------------------------------------
// CPU INFORMATION
// Reads CPU model and number of logical processors
// from /proc/cpuinfo.
// ------------------------------------------------------------

CPUInfo getCPUInfo(){
    ifstream file("/proc/cpuinfo");

    if (!file){
        throw runtime_error("Unable to open /proc/cpuinfo");
    }

    CPUInfo info;
    info.logicalProcessors=0;

    string line;

    while (getline(file, line)){
        if (line.compare(0, 9, "processor")==0){
            info.logicalProcessors++;
        }

        if (info.modelName.empty()&&line.compare(0, 10, "model name")==0){
            size_t pos = line.find(':');

            if (pos!=string::npos){
                info.modelName = line.substr(pos + 1);

                while (!info.modelName.empty() && info.modelName.front()==' '){
                    info.modelName.erase(0, 1);
                }
            }
        }
    }

    if (info.modelName.empty() || info.logicalProcessors == 0){
        throw runtime_error("Unable to read CPU information");
    }

    return info;
}


// ------------------------------------------------------------
// MEMORY INFORMATION
// Reads MemTotal, MemFree and MemAvailable from /proc/meminfo.
// Memory is returned in GB.
// ------------------------------------------------------------

MemoryInfo getMemoryInfo(){
    ifstream file("/proc/meminfo");

    if (!file){
        throw runtime_error("Unable to open /proc/meminfo");
    }

    MemoryInfo info;

    long long totalKB=0;
    long long freeKB=0;
    long long availableKB=0;

    string key;
    long long value;
    string unit;

    while (file >> key >> value >> unit){
        if (key=="MemTotal:"){
            totalKB = value;
        }
        else if (key=="MemFree:"){
            freeKB = value;
        }
        else if (key=="MemAvailable:"){
            availableKB = value;
        }
    }

    if (totalKB==0){
        throw runtime_error("Unable to read memory information");
    }

    // /proc/meminfo reports memory in kB. Convert kB -> GB using 1024 * 1024.
    info.totalGB=totalKB/(1024.0 * 1024.0);
    info.freeGB=freeKB/(1024.0 * 1024.0);
    info.availableGB=availableKB/(1024.0 * 1024.0);

    // MemAvailable is used instead of MemFree because Linux can reclaim cached/buffered memory and make it available to applications.
    info.usagePercentage=(totalKB-availableKB)*100.0/totalKB;

    return info;
}


// ------------------------------------------------------------
// CPU TIME
// Reads the aggregate "cpu" line from /proc/stat.
//
// The values in /proc/stat are cumulative CPU-time counters
// since boot. They are used later to calculate CPU utilization.
// ------------------------------------------------------------

CPUTime getCPUTime()
{
    ifstream file("/proc/stat");

    if (!file)
    {
        throw runtime_error("Unable to open /proc/stat");
    }

    string cpu;

    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;

    file >> cpu
         >> user
         >> nice
         >> system
         >> idle
         >> iowait
         >> irq
         >> softirq
         >> steal;

    if (!file || cpu != "cpu")
    {
        throw runtime_error("Unable to read CPU statistics");
    }

    CPUTime info;

    info.total = user + nice + system + idle +
                 iowait + irq + softirq + steal;

    info.idle = idle + iowait;

    // guest and guest_nice are not included because their CPU
    // time is already accounted for in user and nice respectively.
    // Including them here would double-count CPU time.

    return info;
}


// ------------------------------------------------------------
// CPU UTILIZATION
// Calculates CPU utilization between two /proc/stat samples.
// ------------------------------------------------------------

double calculateCPUUsage(
    const CPUTime& previous,
    const CPUTime& current)
{
    unsigned long long totalDelta =
        current.total - previous.total;

    unsigned long long idleDelta =
        current.idle - previous.idle;

    if (totalDelta == 0)
    {
        return 0.0;
    }

    // /proc/stat contains cumulative CPU counters since boot,
    // so utilization is calculated from the difference between
    // two samples rather than from a single reading.
    return ((totalDelta - idleDelta) * 100.0)
           / totalDelta;
}


// ------------------------------------------------------------
// SYSTEM UPTIME
// Reads the first value from /proc/uptime and converts it to
// Days, Hours and Minutes.
// ------------------------------------------------------------

string getUptime()
{
    ifstream file("/proc/uptime");

    if (!file)
    {
        throw runtime_error("Unable to open /proc/uptime");
    }

    double uptimeSeconds;

    file >> uptimeSeconds;

    if (!file)
    {
        throw runtime_error("Unable to read system uptime");
    }

    long long totalSeconds =
        static_cast<long long>(uptimeSeconds);

    long long days = totalSeconds / 86400;
    totalSeconds %= 86400;

    long long hours = totalSeconds / 3600;
    totalSeconds %= 3600;

    long long minutes = totalSeconds / 60;

    return to_string(days) + " Days " +
           to_string(hours) + " Hours " +
           to_string(minutes) + " Minutes";
}


// ------------------------------------------------------------
// LOAD AVERAGE
// Reads the 1-, 5- and 15-minute load averages from
// /proc/loadavg.
// ------------------------------------------------------------

LoadAverage getLoadAverage()
{
    ifstream file("/proc/loadavg");

    if (!file)
    {
        throw runtime_error("Unable to open /proc/loadavg");
    }

    LoadAverage load;

    file >> load.oneMinute
         >> load.fiveMinutes
         >> load.fifteenMinutes;

    if (!file)
    {
        throw runtime_error("Unable to read load average");
    }

    return load;
}


// ------------------------------------------------------------
// COMPLETE SYSTEM STATISTICS
// Collects CPU utilization, uptime and load averages.
// CPU utilization is measured over a 1-second interval.
// ------------------------------------------------------------

SystemStats getSystemStats()
{
    SystemStats stats;

    CPUTime previous = getCPUTime();

    this_thread::sleep_for(
        chrono::seconds(1)
    );

    CPUTime current = getCPUTime();

    stats.cpuUsage =
        calculateCPUUsage(previous, current);

    stats.uptime = getUptime();

    stats.loadAverage = getLoadAverage();

    return stats;
}
