//
// Created by martin on 10/29/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    float frecuencia;
    float resto_tick;
    ProcessQueue* process_queue;
    int ultimo_pid;
}ProcessGenerator;

void generar_proceso(ProcessGenerator* process_generator) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    init_pcb(pcb, process_generator -> ultimo_pid);
    process_generator -> ultimo_pid++;
    enqueue(process_generator -> process_queue, pcb);
}

/**
 * Debe ejecutar la función de generar proceso
 */
void ejecutar_funcion_temporizador_process_generator(void* self) {
    ProcessGenerator* process_generator = (ProcessGenerator*)self;
    float* resto_tick = &process_generator -> resto_tick;

    *resto_tick += process_generator -> frecuencia;

    while(*resto_tick >= 1) {
        generar_proceso(process_generator);
        print_queue(process_generator -> process_queue);
        *resto_tick -= 1;
    }
}

void init_process_generator(ProcessGenerator* process_generator, ProcessQueue* process_queue, float frecuencia) {
    process_generator -> frecuencia = frecuencia;
    process_generator -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_process_generator;
    process_generator -> process_queue = process_queue;
    process_generator -> ultimo_pid = 0;
    process_generator -> resto_tick = 0;
}
