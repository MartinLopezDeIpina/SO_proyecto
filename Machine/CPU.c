//
// Created by martin on 12/23/24.
//

#include <stdlib.h>
#include <stdio.h>

#include "CPU.h"

void init_CPU(int id_cpu, CPU* cpu, int num_cores) {
    cpu -> id_cpu = id_cpu;
    cpu -> num_cores = num_cores;
    cpu -> cores = (Core*)malloc(num_cores * sizeof(Core));

    for (int i = 0; i < num_cores; i++) {
        printf("iniciando core %d\n", i);
        int id_core = id_cpu * num_cores + i;
        init_core(id_core, &cpu->cores[i]);
    }
}

void notificar_tick_clock_CPU(CPU* cpu) {
    for (int i = 0; i < cpu->num_cores; i++) {
        notificar_tick_clock_core(&cpu->cores[i]);
    }
}

int get_cores_ociosos_CPU(CPU* cpu, int* array_ids, int index_actual) {
   int cont = 0;
   for (int i = 0; i < cpu->num_cores; i++) {
       if (core_esta_ocioso(&cpu -> cores[i]) == TRUE) {
           array_ids[index_actual + cont] = cpu->cores[i].id_core;
           cont++;
       }
   }
   return cont;
}

void vaciar_cpus_terminados(CPU* cpu) {
    for (int i = 0; i < cpu->num_cores; i++) {
        if (!core_esta_ocioso(&cpu->cores[i])) {
            if (proceso_core_ha_terminado(&cpu->cores[i]) == TRUE) {
                vaciar_core(&cpu->cores[i]);
            }
        }
    }
}
