#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "Clock.h"
#include "Temporizador.h"
#include "PCB.h"
#include "Loader.h"
#include "ProcessQueue.h"
#include "Scheduler.h"
#include "Machine/Machine.h"
#include "DebugPrints.h"


void execute_in_different_thread(void* (*func)(void*), void* arg[]) {
    pthread_t thread;
    pthread_create(&thread, NULL, func, arg);
}

int main(int argc, char* argv[]) {

    int FRECUENCIA_CLOCK = atoi(argv[1]);
    float FRECUENCIA_GENERADOR_PROCESOS = atof(argv[2]);
    int NUM_CPUS = atoi(argv[3]);
    int NUM_CORES_CPU = atoi(argv[4]);
    int NUM_THREADS_CORE = atoi(argv[5]);

    Clock clock;
    init_clock(&clock, FRECUENCIA_CLOCK, 3);
    set_global_clock(&clock);

    ProcessQueue pq;
    init_process_queue(&pq);

    PhysicalMemory pm;
    init_physical_memory(&pm);

    Loader pg;
    init_loader(&pg, &pq, &pm);

    Machine machine;
    init_machine(&machine, &pm, NUM_CPUS, NUM_CORES_CPU, NUM_THREADS_CORE);

    Scheduler scheduler;
    init_scheduler(&scheduler, &pq, &machine);

    Temporizador temp1;
    Temporizador temp2;
    Temporizador temp3;

    init_temporizador(&temp1, &pg, &pg.i_componente_temporizable, FRECUENCIA_GENERADOR_PROCESOS);
    init_temporizador(&temp2, &machine, &machine.i_componente_temporizable, 1);
    init_temporizador(&temp3, &scheduler, &scheduler.i_componente_temporizable, 1);

    sleep(1000);
    return 0;
}
