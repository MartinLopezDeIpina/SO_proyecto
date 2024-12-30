//
// Created by martin on 10/29/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    ProcessQueue* process_queue;
    int ultimo_pid;
}Loader;

void generar_proceso(Loader* process_generator) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    int prioridad = int_aleatorio_entre_dos_numeros(1, 3);
    init_pcb(pcb, process_generator -> ultimo_pid, prioridad);
    process_generator -> ultimo_pid++;
    enqueue(process_generator -> process_queue, pcb);
}

void ejecutar_funcion_temporizador_process_generator(void* self) {
    Loader* process_generator = (Loader*)self;

    generar_proceso(process_generator);
}

void init_process_generator(Loader* process_generator, ProcessQueue* process_queue) {
    process_generator -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_process_generator;
    process_generator -> process_queue = process_queue;
    process_generator -> ultimo_pid = 0;

    for(int i = 0; i < 4; i++) {
        generar_proceso(process_generator);
    }
}
