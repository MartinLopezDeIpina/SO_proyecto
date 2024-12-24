//
// Created by martin on 11/3/24.
//

#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "IComponenteTemporizable.h"
#include "Machine/Machine.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    ProcessQueue* process_queue;
    Machine* machine;
}Scheduler;

void funcion_scheduler(Scheduler* scheduler);
void ejecutar_funcion_temporizador(void* self);
void init_scheduler(Scheduler* scheduler, ProcessQueue* process_queue, Machine* machine);

#endif //SCHEDULER_H
