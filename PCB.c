//
// Created by martin on 10/29/24.
//

#include <stdio.h>

#include "Boolean.h"

typedef struct {
    int pid;
    int num_instruccion_actual;
    int num_instrucciones;
} PCB;

void init_pcb(PCB* pcb, int pid) {
    pcb -> pid = pid;
    pcb -> num_instruccion_actual = 0;
    pcb -> num_instrucciones = 5;
}

void ejecutar_instruccion_proceso(PCB* pcb) {
    printf("proceso %d ejecutando instruccion %d\n", pcb -> pid, pcb -> num_instruccion_actual);
    pcb -> num_instruccion_actual++;
}

/*
 Cuando Scheduler mire, si esto resuelve a True entonces esperará al mutex del core para que acabe y entonces lo sacará.
 Así no se pierde el tick de después de la última instrucción.
 */
Boolean proceso_ejecutando_ultima_instruccion(PCB* pcb) {
    return pcb -> num_instruccion_actual >= pcb -> num_instrucciones - 1;
}