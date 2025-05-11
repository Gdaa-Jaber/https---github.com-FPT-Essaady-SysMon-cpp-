cmake_minimum_required(VERSION 3.10)
project(CPUMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cpu_monitor cpu_monitor.cpp)
target_link_libraries(cpu_monitor pdh) 