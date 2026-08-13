#include "../include/monitor.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <termios.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <algorithm>

using namespace std;

volatile sig_atomic_t running = 1;
termios oldTerminal;
MonitorData latestData;
mutex dataMutex;
bool dataReady = false;

void handleSignal(int sig)
{
    if(sig == SIGINT)
        running = 0;
}

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &oldTerminal);

    termios raw = oldTerminal;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminal);
}

void getTerminalSize(int &rows, int &cols)
{
    winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        rows = 24;
        cols = 80;
        return;
    }

    rows = ws.ws_row;
    cols = ws.ws_col;
}

string getOutput(const MonitorData &data)
{
    ostringstream out;
    streambuf *old = cout.rdbuf(out.rdbuf());

    displayMonitorData(data);

    cout.rdbuf(old);

    return out.str();
}

vector<string> splitLines(const string &text)
{
    vector<string> lines;
    string line;
    stringstream ss(text);

    while(getline(ss, line))
        lines.push_back(line);

    return lines;
}

void drawScreen(const vector<string> &lines, int offset)
{
    int rows, cols;
    getTerminalSize(rows, cols);

    cout << "\033[2J\033[H";

    for(int i = 0; i < rows; ++i)
    {
        int index = offset + i;

        if(index >= (int)lines.size())
            break;

        string line = lines[index];

        if((int)line.size() > cols)
            line.resize(cols);

        cout << line;

        if(i < rows - 1)
            cout << '\n';
    }

    cout.flush();
}

void handleInput(int &offset, int rows, int maxScroll, string &buffer)
{
    char temp[1024];

    int n = read(STDIN_FILENO, temp, sizeof(temp));

    if(n <= 0)
        return;

    buffer.append(temp, n);

    while(!buffer.empty())
    {
        if(buffer[0] != '\033')
        {
            char ch = buffer[0];
            buffer.erase(0, 1);

            if(ch == 'q')
            {
                running = 0;
                return;
            }

            if(ch == 'k')
                offset = max(0, offset - 10);

            else if(ch == 'j')
                offset = min(maxScroll, offset + 10);

            continue;
        }

        if(buffer.size() < 3)
            return;

        if(buffer.compare(0, 3, "\033[A") == 0)
        {
            offset = max(0, offset - 10);
            buffer.erase(0, 3);
        }

        else if(buffer.compare(0, 3, "\033[B") == 0)
        {
            offset = min(maxScroll, offset + 10);
            buffer.erase(0, 3);
        }

        else if(buffer.compare(0, 4, "\033[5~") == 0)
        {
            offset = max(0, offset - rows);
            buffer.erase(0, 4);
        }

        else if(buffer.compare(0, 4, "\033[6~") == 0)
        {
            offset = min(maxScroll, offset + rows);
            buffer.erase(0, 4);
        }

        else if(buffer.compare(0, 3, "\033[H") == 0)
        {
            offset = 0;
            buffer.erase(0, 3);
        }

        else if(buffer.compare(0, 3, "\033[F") == 0)
        {
            offset = maxScroll;
            buffer.erase(0, 3);
        }

        else if(buffer.compare(0, 3, "\033[<") == 0)
        {
            size_t end = buffer.find_first_of("Mm", 3);

            if(end == string::npos)
                return;

            string event = buffer.substr(3, end - 3);
            buffer.erase(0, end + 1);

            stringstream ss(event);

            int button, x, y;
            char separator;

            ss >> button >> separator >> x >> separator >> y;

            if(button == 64)
                offset = max(0, offset - 10);

            else if(button == 65)
                offset = min(maxScroll, offset + 10);
        }

        else
        {
            buffer.erase(0, 1);
        }
    }
}

void runScrollableMonitor(int interval)
{
    signal(SIGINT, handleSignal);

    enableRawMode();

    cout << "\033[?1049h";
    cout << "\033[?25l";
    cout << "\033[?1000h";
    cout << "\033[?1006h";
    cout.flush();

    int offset = 0;
    string inputBuffer;

    thread worker([interval]()
    {
        while(running)
        {
            MonitorData data = getMonitorData();

            {
                lock_guard<mutex> lock(dataMutex);
                latestData = data;
                dataReady = true;
            }

            for(int i = 0; i < interval * 10 && running; ++i)
                this_thread::sleep_for(chrono::milliseconds(100));
        }
    });

    while(running)
    {
        MonitorData data;
        bool ready = false;

        {
            lock_guard<mutex> lock(dataMutex);

            if(dataReady)
            {
                data = latestData;
                ready = true;
            }
        }

        if(ready)
        {
            vector<string> lines = splitLines(getOutput(data));

            int rows, cols;
            getTerminalSize(rows, cols);

            int maxScroll = max(0, (int)lines.size() - rows);

            offset = min(offset, maxScroll);

            drawScreen(lines, offset);
        }

        pollfd pfd;
        pfd.fd = STDIN_FILENO;
        pfd.events = POLLIN;

        if(poll(&pfd, 1, 50) > 0)
        {
            if(pfd.revents & POLLIN)
            {
                int rows, cols;
                getTerminalSize(rows, cols);

                int maxScroll = 0;

                {
                    lock_guard<mutex> lock(dataMutex);

                    if(dataReady)
                    {
                        vector<string> lines =
                            splitLines(getOutput(latestData));

                        maxScroll =
                            max(0, (int)lines.size() - rows);
                    }
                }

                handleInput(
                    offset,
                    rows,
                    maxScroll,
                    inputBuffer
                );

                if(running)
                {
                    MonitorData current;

                    {
                        lock_guard<mutex> lock(dataMutex);
                        current = latestData;
                    }

                    vector<string> lines =
                        splitLines(getOutput(current));

                    maxScroll =
                        max(0, (int)lines.size() - rows);

                    offset = min(offset, maxScroll);

                    drawScreen(lines, offset);
                }
            }
        }
    }

    running = 0;

    if(worker.joinable())
        worker.join();

    cout << "\033[?1000l";
    cout << "\033[?1006l";
    cout << "\033[?25h";
    cout << "\033[?1049l";
    cout.flush();

    disableRawMode();
}
