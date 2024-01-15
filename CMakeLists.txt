cmake_minimum_required(VERSION 3.27)
project(ResourceMonitor C)

set(CMAKE_C_STANDARD 11)

include_directories(src)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(ResourceMonitor
        src/main.c
        src/monitor.c
        src/monitor.h
        src/process.c
        src/process.h)

target_link_libraries(ResourceMonitor ${CURSES_LIBRARIES})