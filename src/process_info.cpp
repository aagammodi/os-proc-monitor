#include "../include/process_info.h"

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <cctype>

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

vector<procInfo> getProcessInfo()
{
    vector<procInfo> procData;

    DIR *dir = opendir("/proc");

    if (dir == nullptr)
    {
        cout << "Cannot open /proc" << endl;
        return procData;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != nullptr)
    {
        if (!isNumber(entry->d_name))
            continue;

        procInfo p;

        p.pid = stoi(entry->d_name);

        string path = "/proc/" + string(entry->d_name) + "/status";

        ifstream file(path);

        if (!file)
            continue;

        string line;

        while (getline(file, line))
        {
            if (line.find("Name:") == 0)
            {
                int pos = line.find('\t');
                p.pName = line.substr(pos + 1);
            }

            else if (line.find("State:") == 0)
            {
                int pos = line.find('\t');
                p.procState = line.substr(pos + 1, 1);
            }

            else if (line.find("PPid:") == 0)
            {
                int pos = line.find('\t');
                p.ppid = stoi(line.substr(pos + 1));
            }

            else if (line.find("Threads:") == 0)
            {
                int pos = line.find('\t');
                p.nTh = stoi(line.substr(pos + 1));
            }

            else if (line.find("VmSize:") == 0)
            {
                int pos = line.find('\t');
                p.vmSize = line.substr(pos + 1);
            }
        }

        file.close();

        procData.push_back(p);
    }

    closedir(dir);

    return procData;
}
