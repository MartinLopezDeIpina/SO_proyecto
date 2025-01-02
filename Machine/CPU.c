//
// Created by martin on 12/23/24.
//

#include <stdlib.h>
#include <stdio.h>

#include "CPU.h"

void init_CPU(int id_cpu, CPU* cpu, PhysicalMemory* pm, int num_cores, int num_threads_core) {
    cpu -> id_cpu = id_cpu;
    cpu -> num_cores = num_cores;
    cpu -> cores = (Core*)malloc(num_cores * sizeof(Core));

    for (int i = 0; i < num_cores; i++) {
        printf("iniciando core %d\n", i);
        int id_core = id_cpu * num_cores + i;
        init_core(id_core, &cpu->cores[i], pm, num_threads_core);
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

int vaciar_cpus_terminados(CPU* cpu, int* pid_procesos_terminados, int total_procesos_terminados){
    for (int i = 0; i < cpu->num_cores; i++) {
        total_procesos_terminados += vaciar_hilos_terminados_core(&cpu->cores[i], pid_procesos_terminados, total_procesos_terminados);
    }
    return total_procesos_terminados;
}

void vaciar_cpus_sin_saldo_suficiente(CPU* cpu) {
    for (int i = 0; i < cpu->num_cores; i++) {
        vaciar_hilos_sin_saldo_suficiente(&cpu->cores[i]);
    }
}

void asignar_proceso_a_core_CPU(CPU* cpu, int id_core, PCB* pcb) {
    for (int i = 0; i < cpu->num_cores; i++) {
        if (cpu->cores[i].id_core == id_core) {
            asignar_proceso_a_core(&cpu->cores[i], pcb);
            break;
        }
    }
}
