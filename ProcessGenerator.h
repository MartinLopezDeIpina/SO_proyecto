//
// Created by martin on 10/29/24.
//

#ifndef PROCESSGENERATOR_H
#define PROCESSGENERATOR_H

#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    int frecuencia;
    ProcessQueue* process_queue;
}ProcessGenerator;

void generar_proceso(ProcessGenerator* process_generator);
void ejecutar_funcion_temporizador(void* self);
void init_process_generator(ProcessGenerator* process_generator, ProcessQueue* process_queue, int frecuencia);

#endif //PROCESSGENERATOR_H
