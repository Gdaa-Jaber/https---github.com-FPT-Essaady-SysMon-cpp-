#include "../include/CpuMonitor.h"
#include <fstream>
#include <sstream>
#include <iostream>

CpuMonitor::CpuMonitor() {
    // Lire /proc/stat pour obtenir le nombre de CPUs
    std::ifstream statFile("/proc/stat");
    std::string line;
    numCPUs = 0;
    
    while (std::getline(statFile, line)) {
        if (line.substr(0, 3) == "cpu") {
            numCPUs++;
        }
    }
    numCPUs--; // Soustraire 1 pour la ligne CPU globale

    // Initialiser les vecteurs
    lastTotalUser.resize(numCPUs);
    lastTotalUserLow.resize(numCPUs);
    lastTotalSys.resize(numCPUs);
    lastTotalIdle.resize(numCPUs);

    // Initialiser avec la première lecture
    updateCPUStats();
}

void CpuMonitor::readCPUStats(std::vector<unsigned long long>& stats) {
    std::ifstream statFile("/proc/stat");
    std::string line;
    std::getline(statFile, line); // Ignorer la ligne CPU globale
    
    for (int i = 0; i < numCPUs; i++) {
        std::getline(statFile, line);
        std::istringstream iss(line);
        std::string cpu;
        unsigned long long user, nice, system, idle, iowait, irq, softirq;
        
        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
        
        if (i == 0) { // Ne stocker les stats que pour le premier CPU
            stats = {user, nice, system, idle};
        }
    }
}

void CpuMonitor::updateCPUStats() {
    std::vector<unsigned long long> stats;
    readCPUStats(stats);
    
    for (int i = 0; i < numCPUs; i++) {
        lastTotalUser[i] = stats[0];
        lastTotalUserLow[i] = stats[1];
        lastTotalSys[i] = stats[2];
        lastTotalIdle[i] = stats[3];
    }
}

double CpuMonitor::calculateCPUUsage(int cpuIndex) const {
    std::vector<unsigned long long> stats;
    const_cast<CpuMonitor*>(this)->readCPUStats(stats);
    
    unsigned long long totalUser = stats[0] - lastTotalUser[cpuIndex];
    unsigned long long totalUserLow = stats[1] - lastTotalUserLow[cpuIndex];
    unsigned long long totalSys = stats[2] - lastTotalSys[cpuIndex];
    unsigned long long totalIdle = stats[3] - lastTotalIdle[cpuIndex];
    
    unsigned long long total = totalUser + totalUserLow + totalSys + totalIdle;
    
    return total > 0 ? (total - totalIdle) * 100.0 / total : 0.0;
}

double CpuMonitor::getCPUUsage(int cpuIndex) const {
    if (cpuIndex == -1) {
        // Calculer l'utilisation globale du CPU
        double totalUsage = 0.0;
        for (int i = 0; i < numCPUs; i++) {
            totalUsage += calculateCPUUsage(i);
        }
        return totalUsage / numCPUs;
    }
    
    if (cpuIndex >= 0 && cpuIndex < numCPUs) {
        return calculateCPUUsage(cpuIndex);
    }
    
    return -1.0; // Index CPU invalide
}
