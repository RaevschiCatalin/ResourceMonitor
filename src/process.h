//
// Created by pink on 15.01.2024.
//

#ifndef RESOURCEMONITOR_PROCESS_H
#define RESOURCEMONITOR_PROCESS_H


void get_process_info();
char get_process_status(const char *process_path);

void get_top_processes_by_cpu();
void get_top_processes_by_memory();

#endif //RESOURCEMONITOR_PROCESS_H
