//
// Created by pink on 15.01.2024.
//

#include "monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>

void get_system_info() {
    struct utsname system_info;
    if (uname(&system_info) != 0) {
        perror("Error getting system information");
        return;
    }

    printf("System Information:\n");
    printf("  - System: %s %s %s\n", system_info.sysname, system_info.release, system_info.machine);

    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo != NULL) {
        char line[256];
        int processorCount = 0;
        char *model_name = NULL;
        while (fgets(line, sizeof(line), cpuinfo) != NULL) {
            if (strstr(line, "model name")) {
                if (processorCount > 0) {
                    model_name = strdup(line);
                }
            } else if (strstr(line, "processor")) {
                processorCount++;
            }
        }
        printf("  - %s", model_name);
        printf("  - Total Threads: %d\n", processorCount);
        printf("  - Total Cores: %d\n", processorCount / 2);
        fclose(cpuinfo);
        free(model_name);
    }

    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo != NULL) {
        char line[256];
        int totalMemoryKB = 0;

        while (fgets(line, sizeof(line), meminfo) != NULL) {
            if (strstr(line, "MemTotal")) {
                sscanf(line, "MemTotal: %d kB", &totalMemoryKB);
                break;
            }
        }


        if (totalMemoryKB >= 1024 * 1024) {
            printf("  - Total Memory: %.2f GB\n", totalMemoryKB / (1024.0 * 1024));
        } else {
            printf("  - Total Memory: %.2f MB\n", totalMemoryKB / 1024.0);
        }

        fclose(meminfo);
    }
}


double get_cpu_usage() {

        FILE *statfile = fopen("/proc/stat", "r");

        if (statfile == NULL) {
            perror("Error opening /proc/stat");
            return -1;
        }

        unsigned long long totalUser, totalNice, totalSystem, totalIdle;
        unsigned long long newUser, newNice, newSystem, newIdle;

        if (fscanf(statfile, "cpu %llu %llu %llu %llu", &totalUser, &totalNice, &totalSystem, &totalIdle) != 4) {
            fclose(statfile);
            perror("Error reading /proc/stat");
            return -1;
        }

        fclose(statfile);


        sleep(1);


        statfile = fopen("/proc/stat", "r");

        if (statfile == NULL) {
            perror("Error opening /proc/stat");
            return -1;
        }


        if (fscanf(statfile, "cpu %llu %llu %llu %llu", &newUser, &newNice, &newSystem, &newIdle) != 4) {
            fclose(statfile);
            perror("Error reading /proc/stat");
            return -1;
        }

        fclose(statfile);


        unsigned long long total = totalUser + totalNice + totalSystem + totalIdle;
        unsigned long long newTotal = newUser + newNice + newSystem + newIdle;

        unsigned long long totalUsage = total - totalIdle;
        unsigned long long newTotalUsage = newTotal - newIdle;

        double cpuUsage = (double)(newTotalUsage - totalUsage) / (newTotal - total) * 100.0;

        printf("CPU Usage: %.2f%%\n", cpuUsage);
    printf("CPU Usage: [");
    int barLength = 50;
    int numBars = (int)(cpuUsage / 2.0);
    for (int i = 0; i < barLength; ++i) {
        if (i < numBars) {
            printf("|");
        } else {
            printf(" ");
        }
    }
    printf("]\n");
    return cpuUsage;
    }


    long get_memory_usage() {

        FILE *meminfo = fopen("/proc/meminfo", "r");

        if (meminfo == NULL) {
            perror("Error opening /proc/meminfo");
            return -1;
        }

        unsigned long long totalMemory, freeMemory, usedMemory;

        if (fscanf(meminfo, "MemTotal: %llu kB\nMemFree: %llu kB", &totalMemory, &freeMemory) != 2) {
            fclose(meminfo);
            perror("Error reading /proc/meminfo");
            return -1;
        }

        fclose(meminfo);

        usedMemory = totalMemory - freeMemory;

        if (totalMemory >= 1024 * 1024) {
            printf("Memory Usage: Used %.2f GB, Free %.2f GB, Total %.2f GB\n",
                   usedMemory / (1024.0 * 1024), freeMemory / (1024.0 * 1024), totalMemory / (1024.0 * 1024));
        } else {
            printf("Memory Usage: Used %.2f MB, Free %.2f MB, Total %.2f MB\n",
                   usedMemory / 1024.0, freeMemory / 1024.0, totalMemory / 1024.0);
        }
        printf("Memory Usage: [");
        int barLength = 50;
        int numBars = (int)(usedMemory * barLength / totalMemory);
        for (int i = 0; i < barLength; ++i) {
            if (i < numBars) {
                printf("|");
            } else {
                printf(" ");
            }
        }
        printf("]\n");
        return (double)usedMemory / totalMemory * 100.0;
    }

