//
// Created by martin on 12/23/24.
//

#ifndef CORE_H
#define CORE_H
#include "HiloHardware.h"

#endif //CORE_H
#include <pthread.h>
#include "../PCB.h"
#include "../Boolean.h"

typedef struct {
    int id_core;
    int num_threads_core;
    HiloHardware* hilos;
    pthread_mutex_t mutex_acceso_core;
}Core;

void init_core(int id_core, Core* core, PhysicalMemory* pm, int num_threads_core);

void notificar_tick_clock_core(Core* core);

void asignar_proceso_a_core(Core* core, PCB* pcb);

int vaciar_hilos_terminados_core(Core* core, int* pid_procesos_terminados, int index_actual);
void vaciar_hilos_sin_saldo_suficiente(Core* core);

Boolean core_esta_ocioso(Core* core);
