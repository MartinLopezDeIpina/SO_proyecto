
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "Clock.h"
#include "Temporizador.h"
#include "PCB.h"
#include "ProcessGenerator.h"
#include "ProcessQueue.h"
#include "Scheduler.h"
#include "Machine.h"

void execute_in_different_thread(void* (*func)(void*), void* arg[]) {
    pthread_t thread;
    pthread_create(&thread, NULL, func, arg);
}

int main(int argc, char* argv[]) {


    int FRECUENCIA_CLOCK = atoi(argv[1]);
    int FRECUENCIA_GENERADOR_PROCESOS = atoi(argv[2]);
    int NUM_CPUS = atoi(argv[3]);

    Clock clock;
    init_clock(&clock, FRECUENCIA_CLOCK, 3);

    ProcessQueue pq;
    init_process_queue(&pq);

    ProcessGenerator pg;
    init_process_generator(&pg, &pq, FRECUENCIA_GENERADOR_PROCESOS);

    Machine machine;
    init_machine(&machine, NUM_CPUS);

    Scheduler scheduler;
    init_scheduler(&scheduler);

    Temporizador temp1;
    Temporizador temp2;
    Temporizador temp3;

    init_temporizador(&temp1, &clock, &pg, &pg.i_componente_temporizable);
    sleep(0.01);
    init_temporizador(&temp2, &clock, &machine, &machine.i_componente_temporizable);
    sleep(0.01);
    init_temporizador(&temp3, &clock, &scheduler, &scheduler.i_componente_temporizable);

    /*init_temporizador(&temp1, &clock, &pg, &pg.i_componente_temporizable);
    pthread_t thread;
    pthread_create(&thread, NULL, bucle_temporizador, &temp1);
    //sleep(0.01);
    init_temporizador(&temp2, &clock, &machine, &machine.i_componente_temporizable);
    pthread_t thread2;
    pthread_create(&thread2, NULL, bucle_temporizador, &temp2);
    //sleep(0.01);
    init_temporizador(&temp3, &clock, &scheduler, &scheduler.i_componente_temporizable);
    pthread_t thread3;
    pthread_create(&thread3, NULL, bucle_temporizador, &temp3);
    //sleep(0.01);
    */



    sleep(1000);
    return 0;
}
