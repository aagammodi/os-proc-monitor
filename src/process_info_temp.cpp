#include <bits/stdc++.h>
#include <dirent.h>
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

int isNumber(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

void getPids(vector<int> &pids)
{
    DIR *dir = opendir("/proc");
    if (dir == NULL)
    {
        cout << "Unable to Run!!" << endl;
        return;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        if (isNumber(ent->d_name))
            pids.push_back(atoi(ent->d_name));
    }
    closedir(dir);
}
void getData(ifstream &procFile, vector<procInfo> &procData, int pid)
{
    string line;
    procInfo temp;
    temp.pid = pid;
    int pos;
    while (getline(procFile, line))
    {
        if (line.find("Name:") != string::npos)
        {
            pos = line.find('\t');
            temp.pName = line.substr(pos + 1);
        }
        else if (line.find("State") != string::npos)
        {
            pos = line.find('\t');
            temp.procState = line.substr(pos + 1, 1);
        }
        else if (line.find("PPid") != string::npos)
        {
            pos = line.find('\t');
            temp.ppid = stoi(line.substr(pos + 1));
        }
        else if (line.find("VmSize") != string::npos)
        {
            pos = line.find('\t');
            temp.vmSize = line.substr(pos + 1);
        }
        else if (line.find("Threads") != string::npos)
        {
            pos = line.find('\t');
            temp.nTh = stoi(line.substr(pos + 1));
            break;
        }
    }
    procData.push_back(temp);
}
void createProcData(vector<procInfo> &procData, vector<int> pids)
{
    ifstream sta;
    for (int i = 0; i < pids.size(); i++)
    {
        sta.open("/proc/" + to_string(pids[i]) + "/status");

        getData(sta, procData, pids[i]);
        sta.close();
    }
}

int main()
{
    vector<int> pids;
    vector<procInfo> procData;
    getPids(pids);
    createProcData(procData, pids);
    cout << left
         << setw(8) << "PID"
         << setw(42) << "Name"
         << setw(10) << "State"
         << setw(8) << "PPID"
         << setw(10) << "Threads"
         << setw(12) << "VmSize"
         << endl;

    cout << "-----------------------------------------------------------------------------------------\n";

    for (auto &i : procData)
    {
        cout << left
            <<'|'
             << setw(8) << i.pid
             << setw(42) << i.pName
             << setw(10) << i.procState
             << setw(8) << i.ppid
             << setw(7) << i.nTh
             << setw(12) << i.vmSize
             << '|'
             <<endl;
    }
    cout << "-----------------------------------------------------------------------------------------\n";
    return 0;
}
