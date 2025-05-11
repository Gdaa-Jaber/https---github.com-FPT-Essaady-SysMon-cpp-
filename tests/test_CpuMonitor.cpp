#include "../include/CpuMonitor.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

void testCPUDetection() {
    std::cout << "اختبار كشف المعالج..." << std::endl;
    
    CpuMonitor monitor(80.0); // عتبة 80%
    monitor.updateCPUStats();
    
    // التحقق من وجود معالج واحد على الأقل
    assert(monitor.getNumCPUs() > 0);
    std::cout << "عدد المعالجات المكتشفة: " << monitor.getNumCPUs() << std::endl;
    
    // التحقق من أن استخدام المعالج بين 0 و 100%
    double usage = monitor.getCPUUsage();
    assert(usage >= 0.0 && usage <= 100.0);
    std::cout << "استخدام المعالج الإجمالي: " << usage << "%" << std::endl;
}

void testSpikeDetection() {
    std::cout << "\nاختبار كشف الذروات..." << std::endl;
    
    CpuMonitor monitor(80.0);
    monitor.updateCPUStats();
    
    // التحقق من كشف الذروات
    bool spikeDetected = monitor.checkForSpikes();
    std::cout << "تم كشف ذروة: " << (spikeDetected ? "نعم" : "لا") << std::endl;
    
    // التحقق من التنبيهات
    const auto& alerts = monitor.getAlerts();
    std::cout << "عدد التنبيهات: " << alerts.size() << std::endl;
}

void testCSVExport() {
    std::cout << "\nاختبار تصدير CSV..." << std::endl;
    
    CpuMonitor monitor;
    monitor.updateCPUStats();
    
    // إنشاء ملف مؤقت
    std::string filename = "cpu_stats_test.csv";
    
    try {
        monitor.exportToCSV(filename);
        std::cout << "تم التصدير بنجاح" << std::endl;
        
        // التحقق من وجود الملف
        assert(std::filesystem::exists(filename));
        
        // التحقق من محتوى الملف
        std::ifstream file(filename);
        std::string line;
        std::getline(file, line); // قراءة العنوان
        assert(line == "الوقت,رقم المعالج,استخدام المعالج");
        
        // التنظيف
        std::filesystem::remove(filename);
    }
    catch (const std::exception& e) {
        std::cerr << "خطأ في تصدير CSV: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    try {
        testCPUDetection();
        testSpikeDetection();
        testCSVExport();
        
        std::cout << "\nتم اجتياز جميع الاختبارات بنجاح!" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "خطأ في الاختبارات: " << e.what() << std::endl;
        return 1;
    }
}
