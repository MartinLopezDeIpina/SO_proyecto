#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "Clock.h"
#include "Temporizador.h"
#include "PCB.h"
#include "ProcessGenerator.h"
#include "ProcessQueue.h"
#include "Scheduler.h"


int main(void) {
    ProcessQueue pq;
    init_process_queue(&pq);

    Clock clock;
    init_clock(&clock, 1, 2);

    Temporizador temp1;
    Temporizador temp2;
    Temporizador temp3;

    ProcessGenerator pg;
    init_process_generator(&pg, 1);

    ProcessGenerator pg2;
    init_process_generator(&pg2, 1);

    Scheduler scheduler;
    init_scheduler(&scheduler);

    init_temporizador(&temp1, &clock, &pg, &pg.i_componente_temporizable);
    sleep(0.01);
    init_temporizador(&temp2, &clock, &pg2, &pg2.i_componente_temporizable);
    sleep(0.01);
    init_temporizador(&temp3, &clock, &scheduler, &scheduler.i_componente_temporizable);

    sleep(1000);
    return 0;
}
