//
// Created by pink on 15.01.2024.
//

#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>


#include "monitor.h"
#include "process.h"

int main() {

    while(1){
        system("clear");
        get_system_info();
        get_cpu_usage();
        get_memory_usage();
        get_process_info();
        get_top_processes_by_cpu();
        get_top_processes_by_memory();
        sleep(3);
    }

    return 0;
}
