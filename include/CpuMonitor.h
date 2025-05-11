#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#include <string>
#include <vector>
#include <fstream>

class CpuMonitor {
private:
    std::vector<unsigned long long> lastTotalUser;
    std::vector<unsigned long long> lastTotalUserLow;
    std::vector<unsigned long long> lastTotalSys;
    std::vector<unsigned long long> lastTotalIdle;
    int numCPUs;

    void readCPUStats(std::vector<unsigned long long>& stats);
    double calculateCPUUsage(int cpuIndex);

public:
    CpuMonitor();
    double getCPUUsage(int cpuIndex = -1); // -1 for overall CPU usage
    int getNumCPUs() const { return numCPUs; }
};

#endif // CPUMONITOR_H
