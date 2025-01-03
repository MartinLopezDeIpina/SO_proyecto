//
// Created by martin on 11/3/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "../IComponenteTemporizable.h"
#include "Machine.h"

#include <string.h>

#include "../DebugPrints.h"

char* funcion_machine(Machine* machine) {
    printf("ejeutando funcion machine\n");

    // Ir acumulando el print de todos los cores para printearlo sin interrupciones de otros prints
    char* resultado_print = malloc(10000);
    resultado_print[0] = '\0';

    for (int i = 0; i < machine->num_CPUs; i++) {
        char* resultado_cpu = notificar_tick_clock_CPU(&machine->cpus[i]);
        strcat(resultado_print, resultado_cpu);
        free(resultado_cpu);
    }
    return resultado_print;
}

void ejecutar_funcion_temporizador_machine(void* self) {
    Machine* machine = (Machine*)self;

    char* estado_completo = funcion_machine(machine);
    printf("%s", estado_completo);
    free(estado_completo);
}

void init_machine(Machine* machine, PhysicalMemory* pm, int num_CPUs, int num_cores_CPU, int num_threads_core){
    machine -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_machine;
    machine -> num_CPUs = num_CPUs;
    machine -> num_cores_CPU = num_cores_CPU;
    machine -> num_threads_core = num_threads_core;
    machine -> cpus = (CPU*)malloc(num_CPUs * sizeof(CPU));

    for (int i = 0; i < num_CPUs; i++) {
        printf("iniciando cpu %d\n", i);
        init_CPU(i, &machine->cpus[i],pm, num_cores_CPU, num_threads_core);
    }
}

int get_ids_cores_ociosos(Machine* machine, int* ids_cores_ociosos) {
    int cuenta_total = 0;

    for (int i = 0; i < machine->num_CPUs; i++) {
        cuenta_total += get_cores_ociosos_CPU(&machine->cpus[i], ids_cores_ociosos, cuenta_total);
    }

    return cuenta_total;
}

int get_num_cores_machine(Machine* machine) {
    return machine->num_CPUs * machine->num_cores_CPU;
}

int vaciar_cores_terminados(Machine* machine, int* pid_procesos_terminados) {
    int total_procesos_terminados = 0;
    for (int i = 0; i < machine->num_CPUs; i++) {
        total_procesos_terminados += vaciar_cpus_terminados(&machine->cpus[i], pid_procesos_terminados, total_procesos_terminados);
    }
    return total_procesos_terminados;
}

void vaciar_cores_sin_saldo_suficiente(Machine* machine) {
    for (int i = 0; i < machine->num_CPUs; i++) {
        vaciar_cpus_sin_saldo_suficiente(&machine->cpus[i]);
    }
}

void asignar_proceso_a_machine(Machine* machine, int id_core, PCB* pcb) {
    int id_cpu = id_core / machine->num_cores_CPU;
    asignar_proceso_a_core_CPU(&machine->cpus[id_cpu], id_core, pcb);
}
