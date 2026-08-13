# Linux System Monitoring Tool

## Assignment 1 : Advances in Operating Systems Design

## Team Members

| Roll Number | Name |
|---|---|
| 26CS60R19 | Arunbalaji R |
| 26CS60R33 | Himanshu Parmar |
| 26CS60R61 | Aagam Modi |

## Project Structure

```text
Proc/
├── include/
│   ├── monitor.h
│   ├── process_info.h
│   └── system_info.h
│
├── src/
│   ├── main.cpp
│   ├── monitor.cpp
│   ├── process_info.cpp
|   ├── scroll.cpp
│   └── system_info.cpp
│
├── Makefile
└── README.md
```

## How to Build and Run the Project

### 1. Open the Project Directory

Open a terminal and move to the project directory:

```bash
cd /path/to/your/project
```


### 2. Build the Project

```bash
make
```

### 3. Run in Static Mode

To build and run the program in static mode:

```bash
make run
```

This is equivalent to:

```bash
./proc_monitor
```

The program displays CPU and memory information, CPU utilization and system statistics, the running process list, and the top resource-consuming processes.

### 4. Run in Live Mode

To build and run the program in live mode with a 2-second refresh interval:

```bash
make live
```

This is equivalent to:

```bash
./proc_monitor -T 2
```

The display is refreshed every 2 seconds.

To stop live mode, press:

```text
Ctrl+C
```

### 5. Run Live Mode with a Custom Interval

If a different refresh interval is required, run the executable directly:

```bash
./proc_monitor -T <interval>
```

For example, to refresh every 10 seconds:

```bash
./proc_monitor -T 10
```