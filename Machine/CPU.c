//
// Created by martin on 12/23/24.
//

#include <stdlib.h>
#include <stdio.h>

#include "Core.h"


typedef struct {
    int num_cores;
    Core* cores;
}CPU;

void init_CPU(CPU* cpu, int num_cores) {
    cpu -> num_cores = num_cores;
    cpu -> cores = (Core*)malloc(num_cores * sizeof(Core));

    for (int i = 0; i < num_cores; i++) {
        printf("iniciando core %d\n", i);
        init_core(&cpu->cores[i]);
    }
}

void notificar_tick_clock_CPU(CPU* cpu) {
    for (int i = 0; i < cpu->num_cores; i++) {
        notificar_tick_clock_core(&cpu->cores[i]);
    }
}
