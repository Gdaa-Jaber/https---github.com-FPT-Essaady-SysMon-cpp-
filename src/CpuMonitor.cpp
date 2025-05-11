#include "../include/CpuMonitor.h"
#include <fstream>
#include <sstream>
#include <iostream>

CpuMonitor::CpuMonitor() {
    // Read /proc/stat to get number of CPUs
    std::ifstream statFile("/proc/stat");
    std::string line;
    numCPUs = 0;
    
    while (std::getline(statFile, line)) {
        if (line.substr(0, 3) == "cpu") {
            numCPUs++;
        }
    }
    numCPUs--; // Subtract 1 for the overall CPU line

    // Initialize vectors
    lastTotalUser.resize(numCPUs);
    lastTotalUserLow.resize(numCPUs);
    lastTotalSys.resize(numCPUs);
    lastTotalIdle.resize(numCPUs);

    // Initialize with first reading
    for (int i = 0; i < numCPUs; i++) {
        std::vector<unsigned long long> stats;
        readCPUStats(stats);
        lastTotalUser[i] = stats[0];
        lastTotalUserLow[i] = stats[1];
        lastTotalSys[i] = stats[2];
        lastTotalIdle[i] = stats[3];
    }
}

void CpuMonitor::readCPUStats(std::vector<unsigned long long>& stats) {
    std::ifstream statFile("/proc/stat");
    std::string line;
    std::getline(statFile, line); // Skip overall CPU line
    
    for (int i = 0; i < numCPUs; i++) {
        std::getline(statFile, line);
        std::istringstream iss(line);
        std::string cpu;
        unsigned long long user, nice, system, idle, iowait, irq, softirq;
        
        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
        
        if (i == 0) { // Only store stats for the first CPU
            stats = {user, nice, system, idle};
        }
    }
}

double CpuMonitor::calculateCPUUsage(int cpuIndex) {
    std::vector<unsigned long long> stats;
    readCPUStats(stats);
    
    unsigned long long totalUser = stats[0] - lastTotalUser[cpuIndex];
    unsigned long long totalUserLow = stats[1] - lastTotalUserLow[cpuIndex];
    unsigned long long totalSys = stats[2] - lastTotalSys[cpuIndex];
    unsigned long long totalIdle = stats[3] - lastTotalIdle[cpuIndex];
    
    unsigned long long total = totalUser + totalUserLow + totalSys + totalIdle;
    
    // Update last values
    lastTotalUser[cpuIndex] = stats[0];
    lastTotalUserLow[cpuIndex] = stats[1];
    lastTotalSys[cpuIndex] = stats[2];
    lastTotalIdle[cpuIndex] = stats[3];
    
    return total > 0 ? (total - totalIdle) * 100.0 / total : 0.0;
}

double CpuMonitor::getCPUUsage(int cpuIndex) {
    if (cpuIndex == -1) {
        // Calculate overall CPU usage
        double totalUsage = 0.0;
        for (int i = 0; i < numCPUs; i++) {
            totalUsage += calculateCPUUsage(i);
        }
        return totalUsage / numCPUs;
    }
    
    if (cpuIndex >= 0 && cpuIndex < numCPUs) {
        return calculateCPUUsage(cpuIndex);
    }
    
    return -1.0; // Invalid CPU index
}
