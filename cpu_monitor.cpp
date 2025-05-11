#include <windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

class CPUMonitor {
private:
    PDH_HQUERY query;
    PDH_HCOUNTER counter;
    bool running;

public:
    CPUMonitor() : running(false) {
        // Initialize PDH query
        PdhOpenQuery(NULL, 0, &query);
        // Add CPU counter
        PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter);
    }

    ~CPUMonitor() {
        if (query) {
            PdhCloseQuery(query);
        }
    }

    void start() {
        running = true;
        while (running) {
            PDH_FMT_COUNTERVALUE value;
            
            // Collect data
            PdhCollectQueryData(query);
            
            // Get the formatted value
            PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value);
            
            // Display CPU usage
            std::cout << "CPU Usage: " << value.doubleValue << "%" << std::endl;
            
            // Wait for 1 second
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void stop() {
        running = false;
    }
};

int main() {
    CPUMonitor monitor;
    std::cout << "Starting CPU monitoring... Press Enter to stop." << std::endl;
    
    // Start monitoring in a separate thread
    std::thread monitorThread(&CPUMonitor::start, &monitor);
    
    // Wait for user input to stop
    std::cin.get();
    
    // Stop monitoring
    monitor.stop();
    monitorThread.join();
    
    return 0;
} 