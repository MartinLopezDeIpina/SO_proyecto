//
// Created by martin on 10/29/24.
//

#ifndef PCB_H
#define PCB_H
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

void init_pcb(PCB* pcb, int pid);
void ejecutar_instruccion_proceso(PCB* pcb);
void set_estado_proceso_ejecutando(PCB* pcb);
Boolean proceso_ha_terminado(PCB* pcb);



#endif //PCB_H
