#include "../include/CpuMonitor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

CpuMonitor::CpuMonitor(double spikeThreshold) : spikeThreshold(spikeThreshold) {
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

bool CpuMonitor::checkForSpikes() {
    bool spikeDetected = false;
    
    for (int i = 0; i < numCPUs; i++) {
        double usage = getCPUUsage(i);
        if (usage > spikeThreshold) {
            CPUAlert alert;
            alert.cpuIndex = i;
            alert.usage = usage;
            alert.timestamp = std::chrono::system_clock::now();
            alerts.push_back(alert);
            spikeDetected = true;
        }
    }
    
    return spikeDetected;
}

std::string CpuMonitor::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void CpuMonitor::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier CSV");
    }

    // En-tête CSV
    file << "Timestamp,CPU Index,Utilisation CPU\n";

    // Données actuelles
    std::string timestamp = getTimestamp();
    for (int i = 0; i < numCPUs; i++) {
        file << timestamp << "," << i << "," << getCPUUsage(i) << "\n";
    }

    // Alertes
    for (const auto& alert : alerts) {
        auto time = std::chrono::system_clock::to_time_t(alert.timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        file << ss.str() << "," << alert.cpuIndex << "," << alert.usage << ",ALERTE\n";
    }
}
