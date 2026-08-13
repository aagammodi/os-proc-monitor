#include "../include/process_info.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>
#include <cctype>
#include <unistd.h>

using namespace std;

// Helper function to check if a string is a number
bool isNumber(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return false;
    }

    return true;
}

// Gets information about all processes and returns it in a vector of ProcessInfo structs
vector<ProcessInfo> getProcessInfo()
{
    vector<ProcessInfo> processInfo;

    DIR *dir = opendir("/proc");

    if (dir == nullptr)
    {
        throw runtime_error("Cannot open /proc");
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != nullptr)
    {
        if (!isNumber(entry->d_name))
            continue;

        ProcessInfo p;

        p.pid = stoi(entry->d_name);

        string path = "/proc/" + string(entry->d_name) + "/status";

        ifstream file(path);

        if (!file)
            continue;

        string line;
        int pos;

        while (getline(file, line))
        {
            if (line.find("Name:") == 0)
            {
                pos = line.find('\t');
                p.name = line.substr(pos + 1);
            }

            else if (line.find("State:") == 0)
            {
                pos = line.find('\t');
                p.state = line.substr(pos + 1, 1);
            }

            else if (line.find("PPid:") == 0)
            {
                pos = line.find('\t');
                p.ppid = stoi(line.substr(pos + 1));
            }

            else if (line.find("Threads:") == 0)
            {
                pos = line.find('\t');
                p.threads = stoi(line.substr(pos + 1));
            }

            else if (line.find("VmSize:") == 0)
            {
                int pos = line.find('\t');
                p.vmSizeKB = stoi(line.substr(pos + 1)); // Already in KB
            }
            
            else if (line.find("VmRSS:") == 0)
            {
                int pos = line.find('\t');
                p.vmRSSKB = stoi(line.substr(pos + 1)); // Already in KB
            }
        }

        file.close();

        processInfo.push_back(p);
    }

    closedir(dir);

    return processInfo  ;
}

// Samples the CPU usage of each process over a 1-second interval
void sampleProcesses(vector<ProcessInfo> &processes, int logicalCPUs)
{
    unsigned long long systemStart = getSystemCPUTime();

    for (auto& p : processes)
        p.cpuStartTime = getProcessCPUTime(p.pid);

    sleep(1); // Wait for 1 second

    for (auto& p : processes)
        p.cpuEndTime = getProcessCPUTime(p.pid);

    unsigned long long systemEnd = getSystemCPUTime();

    unsigned long long systemDelta = systemEnd - systemStart;

    for (auto& p : processes)
    {
        if (systemDelta == 0 || p.cpuEndTime < p.cpuStartTime)
            p.cpuUsage = 0;
        else
            p.cpuUsage =
                (double)(p.cpuEndTime - p.cpuStartTime ) /
                systemDelta * logicalCPUs * 100.0;
    }
}

// Gets the total CPU time of the system from /proc/stat
unsigned long long getSystemCPUTime()
{
    ifstream file("/proc/stat");
    if (!file)
        return 0;

    string cpu;
    unsigned long long value, total = 0;

    file >> cpu;

    for (int i = 0; i < 8 && file >> value; ++i)
        total += value;

    return total;
}

// Gets the total CPU time of a process from /proc/[pid]/stat
unsigned long long getProcessCPUTime(int pid)
{
    std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
    if (!file) 
        return 0;

    string line;
    getline(file, line);

    // Skip to the part after the process name
    size_t pos = line.rfind(')');
    if (pos == string::npos)
        return 0;

    istringstream ss(line.substr(pos + 2));

    //field 3: process state
    char state;
    ss >> state;

    // Skip fields 4-13
    unsigned long long value;
    for (int i = 4; i <= 13; ++i)
        ss >> value;

    // Get utime (field 14) and stime (field 15)
    unsigned long long utime, stime;
    ss >> utime >> stime;

    return utime + stime;
}
