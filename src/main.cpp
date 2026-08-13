#include "../include/monitor.h"

#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

int main(int argc, char* argv[])
{
    try
    {
        // Static mode
        if(argc == 1)
        {
            runOnce();
            return 0;
        }

        // Live mode
        if(argc == 3 && string(argv[1]) == "-T")
        {
            int interval = stoi(argv[2]);

            if(interval <= 0)
            {
                throw runtime_error("interval must be greater than 0");
            }

            runLive(interval);
            return 0;
        }

        // Invalid input
        cerr << "Usage:\n";
        cerr << "  " << argv[0] << "\n";
        cerr << "  " << argv[0] << " -T <interval>\n";

        return 1;
    }
    catch(const exception& e)
    {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
