//
// Created by martin on 12/23/24.
//

#ifndef CORE_H
#define CORE_H

#endif //CORE_H
#include <pthread.h>
#include "../PCB.h"
#include "../Boolean.h"

typedef struct {
    int id_core;
    PCB* current_process;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    pthread_mutex_t mutex_acceso_core;
}Core;

void init_core(int id_core, Core* core);

void ejecutar_instruccion(Core* core);

void *funcion_core(void* arg);

void notificar_tick_clock_core(Core* core);

void vaciar_core(Core* core);
void vaciar_core_y_set_estado(Core* core, EstadoProceso estado);

void asignar_proceso_a_core(Core* core, PCB* pcb);

Boolean proceso_core_saldo_ejecucion_insuficiente(Core* core);
Boolean core_esta_vacio(Core* core);
Boolean core_esta_ocioso(Core* core);
Boolean proceso_core_ha_terminado(Core* core);

