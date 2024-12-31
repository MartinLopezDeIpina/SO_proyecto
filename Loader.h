//
// Created by martin on 10/29/24.
//

#ifndef PROCESSGENERATOR_H
#define PROCESSGENERATOR_H

#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"
#include "Machine/PhysicalMemory.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    ProcessQueue* process_queue;
    PhysicalMemory* pm;
    int ultimo_pid;
}Loader;

void generar_proceso(Loader* loader);
void ejecutar_funcion_temporizador(void* self);
void init_loader(Loader* process_generator, ProcessQueue* process_queue, PhysicalMemory* pm);

#endif //PROCESSGENERATOR_H
