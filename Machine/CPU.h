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
void vaciar_cpus_terminados(CPU* cpu);
int get_cores_ociosos_CPU(CPU* cpu, int* array_ids, int index_actual);


