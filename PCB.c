//
// Created by martin on 10/29/24.
//

#include <stdio.h>

#include "Boolean.h"
typedef enum {
    LISTO,
    EJECUTANDO,
    ESPERANDO,
    TERMINADO
} EstadoProceso;

typedef struct {
    int pid;
    int num_instruccion_actual;
    int num_instrucciones;
    EstadoProceso estado;
} PCB;

void init_pcb(PCB* pcb, int pid) {
    pcb -> pid = pid;
    pcb -> num_instruccion_actual = 0;
    pcb -> num_instrucciones = 5;
    pcb -> estado = LISTO;
}

void ejecutar_instruccion_proceso(PCB* pcb) {
    printf("proceso %d ejecutando instruccion %d\n", pcb -> pid, pcb -> num_instruccion_actual);
    pcb -> num_instruccion_actual++;
    if (pcb -> num_instruccion_actual >= pcb -> num_instrucciones) {
        pcb -> estado = TERMINADO;
    }
}

Boolean proceso_ha_terminado(PCB* pcb) {
    if (pcb -> estado == TERMINADO) {
        return TRUE;
    }
}

void set_estado_proceso_ejecutando(PCB* pcb) {
    pcb -> estado = EJECUTANDO;
}