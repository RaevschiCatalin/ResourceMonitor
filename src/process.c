//
// Created by pink on 15.01.2024.
//

// process.c

#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PROCESSES 20


struct ProcessInfo {
    char name[256];
    int pid;
    long value;
};

int compare_processes(const void *a, const void *b) {
    return ((struct ProcessInfo *)b)->value - ((struct ProcessInfo *)a)->value;
}
void get_process_info() {
    DIR *proc_dir = opendir("/proc");

    if (proc_dir == NULL) {
        perror("Error opening /proc directory");
        return;
    }

    int total_processes = 0;

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL) {

        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            total_processes++;
        }
    }

    printf("Process Information:\n");
    printf("  - Total Processes: %d\n", total_processes);

    closedir(proc_dir);
}

void get_top_processes_by_cpu() {
    printf("Top Processes by CPU:\n");

    struct ProcessInfo processes[MAX_PROCESSES];
    int process_count = 0;

    FILE *stat_file = fopen("/proc/stat", "r");
    if (stat_file == NULL) {
        perror("Error opening /proc/stat");
        return;
    }


    char line[256];
    fgets(line, sizeof(line), stat_file);
    fclose(stat_file);


    long total_cpu_time = 0;
    long user, nice, system, idle;
    sscanf(line, "cpu %ld %ld %ld %ld", &user, &nice, &system, &idle);
    total_cpu_time = user + nice + system + idle;


    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("Error opening /proc directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL && process_count < MAX_PROCESSES) {

        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            char stat_path[512];
            snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);

            FILE *process_stat_file = fopen(stat_path, "r");
            if (process_stat_file != NULL) {

                fgets(line, sizeof(line), process_stat_file);
                fclose(process_stat_file);


                char process_name[256];
                long process_user_time, process_system_time;
                sscanf(line, "%*d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %ld %ld",
                       process_name, &process_user_time, &process_system_time);


                long process_cpu_time = process_user_time + process_system_time;


                long process_cpu_usage = (process_cpu_time * 100) / total_cpu_time;


                snprintf(processes[process_count].name, sizeof(processes[process_count].name), "%s", process_name);
                processes[process_count].pid = atoi(entry->d_name);
                processes[process_count].value = process_cpu_usage;

                process_count++;
            }
        }
    }

    closedir(proc_dir);

    qsort(processes, process_count, sizeof(struct ProcessInfo), compare_processes);


    for (int i = 0; i < process_count && i < 5; i++) {
        printf("  - Process: %s, PID: %d, CPU Usage: %ld%%\n",
               processes[i].name, processes[i].pid, processes[i].value);
    }
}

void get_top_processes_by_memory() {
    printf("Top Processes by Memory:\n");

    struct ProcessInfo processes[MAX_PROCESSES];
    int process_count = 0;

    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("Error opening /proc directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL && process_count < MAX_PROCESSES) {

        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            char statm_path[512];
            snprintf(statm_path, sizeof(statm_path), "/proc/%s/statm", entry->d_name);

            FILE *process_statm_file = fopen(statm_path, "r");
            if (process_statm_file != NULL) {

                char line[256];

                fgets(line, sizeof(line), process_statm_file);
                fclose(process_statm_file);


                char process_name[256];
                long process_memory;
                sscanf(line, "%*d %ld", &process_memory);


                snprintf(processes[process_count].name, sizeof(processes[process_count].name), "%s", entry->d_name);
                processes[process_count].pid = atoi(entry->d_name);
                processes[process_count].value = process_memory;

                process_count++;
            }
        }
    }

    closedir(proc_dir);


    qsort(processes, process_count, sizeof(struct ProcessInfo), compare_processes);

    for (int i = 0; i < process_count && i < 5; i++) {
        printf("  - Process: %s, PID: %d, Memory Usage: %ld KB\n",
               processes[i].name, processes[i].pid, processes[i].value);
    }
}

