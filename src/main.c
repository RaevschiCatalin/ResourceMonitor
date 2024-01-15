//
// Created by pink on 15.01.2024.
//

#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>


#include "monitor.h"
#include "process.h"
#include "logger.h"

int main() {
    FILE *log_file = fopen("log.txt", "wb");
    if (log_file == NULL) {
        perror("Error opening log file");
        return 1;
    }

    while (1) {
        system("clear");
        log_system_metrics(log_file);
        printf("______________________________________________________________\n");
        get_system_info();
        printf("______________________________________________________________\n");
        get_cpu_usage();
        printf("______________________________________________________________\n");
        get_memory_usage();
        printf("______________________________________________________________\n");
        int total_processes = get_process_info();
        printf("______________________________________________________________\n");
        get_top_processes_by_memory(total_processes);
        printf("______________________________________________________________\n");
        get_top_processes_by_cpu(total_processes);
        printf("______________________________________________________________\n");
        sleep(10);
    }
    fclose(log_file);

    return 0;
}

