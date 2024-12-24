//
// Created by martin on 11/3/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"
#include "Machine/Machine.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    ProcessQueue* process_queue;
    Machine* machine;
}Scheduler;

void funcion_scheduler(Scheduler* scheduler) {
    //printf("ejeutando funcion scheduler\n");
    vaciar_cores_terminados(scheduler->machine);

    int* ids_cores_ociosos = (int*)malloc(get_num_cores_machine(scheduler->machine) * sizeof(int));
    int num_cores_ociosos = get_ids_cores_ociosos(scheduler->machine, ids_cores_ociosos);

    for (int i = 0; i < num_cores_ociosos; i++) {
        printf("core ocioso: %d\n", ids_cores_ociosos[i]);
    }

}

void ejecutar_funcion_temporizador_scheduler(void* self) {
    Scheduler* scheduler = (Scheduler*)self;
    funcion_scheduler(scheduler);
}

void init_scheduler(Scheduler* scheduler, ProcessQueue* process_queue, Machine* machine) {
    scheduler -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_scheduler;
    scheduler -> process_queue = process_queue;
    scheduler -> machine = machine;
}
