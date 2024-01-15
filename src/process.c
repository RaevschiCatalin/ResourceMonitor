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
int get_process_info() {
    DIR *proc_dir = opendir("/proc");

    if (proc_dir == NULL) {
        perror("Error opening /proc directory");
        return -1;
    }

    int total_processes = 0;

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL) {

        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            total_processes++;
        }
    }

    printf("  - Total Processes: %d\n", total_processes);

    closedir(proc_dir);
    return total_processes;
}

void get_top_processes_by_cpu(int total_processes) {
    printf("Top Processes by CPU:\n");


    struct ProcessInfo *processes = malloc(total_processes * sizeof(struct ProcessInfo));
    if (processes == NULL) {
        perror("Error allocating memory");
        return;
    }

    int process_count = 0;

    FILE *stat_file = fopen("/proc/stat", "r");
    if (stat_file == NULL) {
        perror("Error opening /proc/stat");
        free(processes);
        return;
    }

    char line[256];

    fgets(line, sizeof(line), stat_file);

    long total_cpu_time = 0;


    if (fgets(line, sizeof(line), stat_file) && strncmp(line, "cpu", 3) == 0) {
        long user, nice, system, idle;
        sscanf(line, "cpu %ld %ld %ld %ld", &user, &nice, &system, &idle);
        total_cpu_time = user + nice + system + idle;
    }

    fclose(stat_file);

    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("Error opening /proc directory");
        free(processes);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL && process_count < total_processes) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            char stat_path[512];
            snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);

            FILE *process_stat_file = fopen(stat_path, "r");
            if (process_stat_file != NULL) {

                char *line = malloc(256);
                if (line == NULL) {
                    perror("Error allocating memory");
                    fclose(process_stat_file);
                    closedir(proc_dir);
                    free(processes);
                    return;
                }

                fgets(line, 256, process_stat_file);
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


                free(line);

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


    free(processes);
}



void get_top_processes_by_memory(int total_processes) {
    printf("Top Processes by Memory:\n");
    struct ProcessInfo *processes = malloc(total_processes * sizeof(struct ProcessInfo));
    if (processes == NULL) {
        perror("Error allocating memory");
        return;
    }

    int process_count = 0;

    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("Error opening /proc directory");
        free(processes);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL) {

        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            char stat_path[512];
            snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);

            FILE *process_stat_file = fopen(stat_path, "r");
            if (process_stat_file != NULL) {

                char line[256];
                fgets(line, sizeof(line), process_stat_file);
                fclose(process_stat_file);
                char process_name[256];
                int pid;
                long rss_memory;
                sscanf(line, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*u %ld",
                       &pid, process_name, &rss_memory);
                snprintf(processes[process_count].name, sizeof(processes[process_count].name), "%s", process_name);
                processes[process_count].pid = pid;
                processes[process_count].value = rss_memory;

                process_count++;
            }
        }
    }

    closedir(proc_dir);

    qsort(processes, process_count, sizeof(struct ProcessInfo), compare_processes);

    for (int i = 0; i < process_count && i < 5; i++) {
        printf("  - Process: %s, PID: %d, Memory Usage: %ld MB\n",
               processes[i].name, processes[i].pid, processes[i].value / 1024);
    }

    free(processes);
}
