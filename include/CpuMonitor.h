#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>

struct CPUAlert {
    int cpuIndex;
    double usage;
    std::chrono::system_clock::time_point timestamp;
};

class CpuMonitor {
private:
    std::vector<unsigned long long> lastTotalUser;
    std::vector<unsigned long long> lastTotalUserLow;
    std::vector<unsigned long long> lastTotalSys;
    std::vector<unsigned long long> lastTotalIdle;
    int numCPUs;
    double spikeThreshold;
    std::vector<CPUAlert> alerts;

    void readCPUStats(std::vector<unsigned long long>& stats);
    double calculateCPUUsage(int cpuIndex) const;
    std::string getTimestamp() const;

public:
    CpuMonitor(double spikeThreshold = 80.0);
    void updateCPUStats();
    double getCPUUsage(int cpuIndex = -1) const; // -1 pour l'utilisation globale du CPU
    int getNumCPUs() const { return numCPUs; }
    
    // Nouvelles méthodes
    bool checkForSpikes(); // Retourne true si un pic est détecté
    const std::vector<CPUAlert>& getAlerts() const { return alerts; }
    void exportToCSV(const std::string& filename) const;
    void clearAlerts() { alerts.clear(); }
};

#endif // CPUMONITOR_H
