//
// Created by martin on 12/23/24.
//

#ifndef CPU_H
#define CPU_H
#include "Core.h"

#endif //CPU_H


typedef struct {
    int num_cores;
    Core* cores;
}CPU;

void init_CPU(CPU* cpu, int num_cores);

void notificar_tick_clock_CPU(CPU* cpu);

