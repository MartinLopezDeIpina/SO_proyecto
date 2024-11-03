//
// Created by martin on 10/29/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    //todo: puede ser la frecuencia menor a 1? si eso hacer una variable resto que se vaya acumulando y cuando eso generarlo
    int frecuencia;
    ProcessQueue* process_queue;
    int ultimo_pid;
}ProcessGenerator;

void generar_proceso(ProcessGenerator* process_generator) {
    printf("Generando proceso\n");
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    init_pcb(pcb, process_generator -> ultimo_pid);
    process_generator -> ultimo_pid++;
    enqueue(process_generator -> process_queue, pcb);
    printf("estado actual de la cola de procesos:\n");
    print_queue(process_generator -> process_queue);
}

void ejecutar_funcion_temporizador_process_generator(void* self) {
    ProcessGenerator* process_generator = (ProcessGenerator*)self;
    if(process_generator -> frecuencia >= 1) {
       for (int i = 0; i < process_generator -> frecuencia; i++) {
           generar_proceso(process_generator);
       }
    }
}

void init_process_generator(ProcessGenerator* process_generator, ProcessQueue* process_queue, int frecuencia) {
    process_generator -> frecuencia = frecuencia;
    process_generator -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_process_generator;
    process_generator -> process_queue = process_queue;
    process_generator -> ultimo_pid = 0;
}
