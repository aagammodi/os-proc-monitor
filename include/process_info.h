#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <string>
#include <vector>

using namespace std;

struct procInfo
{
    int pid;
    string pName;
    int ppid;
    string procState;
    int nTh;
    string vmSize;
};

vector<procInfo> getProcessInfo();

#endif
