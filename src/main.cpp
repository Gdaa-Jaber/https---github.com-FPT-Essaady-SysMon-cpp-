#include "../include/CpuMonitor.h"
#include <iostream>
#include <windows.h>
#include <iomanip>

using namespace std;

void printCPUUsage(CpuMonitor& monitor) {
    monitor.updateCPUStats();
    
    cout << "\033[2J\033[1;1H"; // Effacer l'écran et déplacer le curseur en haut à gauche
    cout << "=== Moniteur d'Utilisation CPU ===" << endl;
    
    // Afficher l'utilisation globale du CPU
    cout << "Utilisation Globale CPU: " << fixed << setprecision(1) 
              << monitor.getCPUUsage() << "%" << endl;
    
    // Afficher l'utilisation individuelle du CPU
    cout << "\nUtilisation CPU par Cœur:" << endl;
    for (int i = 0; i < monitor.getNumCPUs(); i++) {
        cout << "CPU " << i << ": " << fixed << setprecision(1) 
                  << monitor.getCPUUsage(i) << "%" << endl;
    }
}

int main() {
    CpuMonitor monitor;
    
    cout << "Démarrage du Moniteur CPU. Appuyez sur Ctrl+C pour quitter." << endl;
    Sleep(1000);
    
    while (true) {
        printCPUUsage(monitor);
        Sleep(1000);
    }
    
    return 0;
}
