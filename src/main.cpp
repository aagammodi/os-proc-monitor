#include "../include/monitor.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    try
    {
        //static mode
        if(argc==1)
        {
            runOnce();
            return 0;
        }

        //Live Mode
        if(argc == 3 && string(argv[1]) == "-T"){

            int interval = stoi(argv[2]);
            if(interval <= 0){
                throw runtime_error("interval must be greater than 0");
            }

            runLive(interval);
            return 0;
        }

        // Invalid input
        std::cerr << "Usage:\n";
        std::cerr << "  " << argv[0] << "\n";
        std::cerr << "  " << argv[0] << " -T <interval>\n";

        return 1;
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
