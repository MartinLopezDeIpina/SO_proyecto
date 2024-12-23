//
// Created by martin on 11/3/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "../IComponenteTemporizable.h"
#include "CPU.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    int num_CPUs;
    int num_cores_CPU;
    int num_threads_core;
    CPU* cpus;
}Machine;

void funcion_machine(Machine* machine) {
    printf("ejeutando funcion machine\n");

    for (int i = 0; i < machine->num_CPUs; i++) {
        notificar_tick_clock_CPU(&machine->cpus[i]);
    }
}

void ejecutar_funcion_temporizador_machine(void* self) {
    Machine* machine = (Machine*)self;
    funcion_machine(machine);
}

void init_machine(Machine* machine, int num_CPUs, int num_cores_CPU, int num_threads_core){
    machine -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_machine;
    machine -> num_CPUs = num_CPUs;
    machine -> num_cores_CPU = num_cores_CPU;
    machine -> num_threads_core = num_threads_core;
    machine -> cpus = (CPU*)malloc(num_CPUs * sizeof(CPU));

    for (int i = 0; i < num_CPUs; i++) {
        printf("iniciando cpu %d\n", i);
        init_CPU(&machine->cpus[i], num_cores_CPU);
    }
}
