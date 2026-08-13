#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <string>
#include <vector>

using namespace std;

struct ProcessInfo {
    int pid = -1;
    int ppid = -1;

    string name = "";
    string state = "";
    int threads = 0;

    long vmSizeKB = 0;
    long vmRSSKB = 0;

    unsigned long long cpuStartTime = 0;
    unsigned long long cpuEndTime = 0;
    double cpuUsage = 0.0;
};

vector<ProcessInfo> getProcessInfo();

void sampleProcesses(vector<ProcessInfo> &processes, int logicalCPUs);
unsigned long long getSystemCPUTime();
unsigned long long getProcessCPUTime(int pid);

#endif
