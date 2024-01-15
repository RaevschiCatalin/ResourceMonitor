//
// Created by pink on 15.01.2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "monitor.h"
#include "logger.h"

struct SystemMetrics {
    time_t timestamp;
    double cpu_usage;
    long memory_usage;
};

void log_system_metrics(FILE *log_file) {
    struct SystemMetrics metrics;
    metrics.timestamp = time(NULL);
    metrics.cpu_usage = get_cpu_usage();
    metrics.memory_usage = get_memory_usage();
    fwrite(&metrics, sizeof(struct SystemMetrics), 1, log_file);
}

void read_historical_data(FILE *log_file) {
    fseek(log_file, 0, SEEK_SET);
    struct SystemMetrics metrics;
    while (fread(&metrics, sizeof(struct SystemMetrics), 1, log_file) == 1) {
        printf("Timestamp: %ld, CPU Usage: %f, Memory Usage: %ld\n",
               metrics.timestamp, metrics.cpu_usage, metrics.memory_usage);
    }
}

