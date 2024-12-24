//
// Created by martin on 10/29/24.
//

#ifndef PCB_H
#define PCB_H
#include "Boolean.h"

typedef struct {
    int pid;
} PCB;

void init_pcb(PCB* pcb, int pid);

void ejecutar_instruccion_proceso(PCB* pcb);

Boolean proceso_ejecutando_ultima_instruccion(PCB* pcb);

#endif //PCB_H
