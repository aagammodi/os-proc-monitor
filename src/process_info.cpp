#include "../include/process_info.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>
#include <cctype>
#include <unistd.h>

using namespace std;

bool isNumber(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return false;
    }

    return true;
}

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
                p.vmSizeMB = stoi(line.substr(pos + 1)) / 1024; // Convert to MB
            }
            
            else if (line.find("VmRSS:") == 0)
            {
                int pos = line.find('\t');
                p.vmRSSMB = stoi(line.substr(pos + 1)) / 1024; // Convert to MB
            }
        }

        file.close();

        processInfo.push_back(p);
    }

    closedir(dir);

    return processInfo  ;
}


void sampleProcesses(vector<ProcessInfo> &processes, int logicalCPUs)
{
    unsigned long long systemStart = getSystemCPUTime();

    for (auto& p : processes)
        p.cpuStartTime = getProcessCPUTime(p.pid);

    sleep(1); // Wait for 1 second

    unsigned long long systemEnd = getSystemCPUTime();

    for (auto& p : processes)
        p.cpuEndTime = getProcessCPUTime(p.pid);

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

unsigned long long getProcessCPUTime(int pid)
{
    std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
    if (!file) 
        return 0;

    string line;
    getline(file, line);

    size_t pos = line.rfind(')');
    if (pos == std::string::npos)
        return 0;

    istringstream ss(line.substr(pos + 2));

    char state;
    unsigned long long value[13];

    ss >> state;

    for (auto& v : value)
        ss >> v;

    if (ss.fail())
        return 0;

    return value[10] + value[11]; // utime + stime
}
