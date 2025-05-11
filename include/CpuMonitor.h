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
    double calculateCPUUsage(int cpuIndex) const;

public:
    CpuMonitor();
    void updateCPUStats();
    double getCPUUsage(int cpuIndex = -1) const; // -1 pour l'utilisation globale du CPU
    int getNumCPUs() const { return numCPUs; }
};

#endif // CPUMONITOR_H
