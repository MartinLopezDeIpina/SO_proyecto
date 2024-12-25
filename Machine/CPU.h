//
// Created by martin on 12/23/24.
//

#ifndef CPU_H
#define CPU_H
#include "Core.h"

#endif //CPU_H


typedef struct{
    int id_cpu;
    int num_cores;
    Core* cores;
}CPU;

void init_CPU(int id_cpu, CPU* cpu, int num_cores);
void notificar_tick_clock_CPU(CPU* cpu);
int vaciar_cpus_terminados(CPU* cpu, int* pid_procesos_terminados, int index_actual);
int get_cores_ociosos_CPU(CPU* cpu, int* array_ids, int index_actual);
void asignar_proceso_a_core_CPU(CPU* cpu, int id_core, PCB* pcb);


