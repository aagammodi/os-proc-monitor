#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <string>
#include <vector>

using namespace std;

struct ProcessInfo{
    int pid;
    int ppid;

    string name;
    string state;
    int threads;

    long vmSizeMB;
    long vmRSSMB;

    unsigned long long cpuStartTime;
    unsigned long long cpuEndTime;
    double cpuUsage;
};

vector<ProcessInfo> getProcessInfo();

void sampleProcesses(vector<ProcessInfo> &processes, int logicalCPUs);
unsigned long long getSystemCPUTime();
unsigned long long getProcessCPUTime(int pid);

#endif
