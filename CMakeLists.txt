cmake_minimum_required(VERSION 3.10)
project(SysMon-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/CpuMonitor.cpp
)

# Add header files
set(HEADERS
    include/CpuMonitor.h
)

# Create executable
add_executable(SysMon-cpp ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(SysMon-cpp PRIVATE include)

# Add tests
enable_testing()
add_executable(test_CpuMonitor tests/test_CpuMonitor.cpp src/CpuMonitor.cpp)
target_include_directories(test_CpuMonitor PRIVATE include)
add_test(NAME CpuMonitorTest COMMAND test_CpuMonitor) 