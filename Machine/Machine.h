//
// Created by martin on 11/3/24.
//

#ifndef MACHINE_H
#define MACHINE_H
#include "CPU.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    int num_CPUs;
    int num_cores_CPU;
    int num_threads_core;
    CPU* cpus;
}Machine;

char* funcion_machine(Machine* machine);
void ejecutar_funcion_temporizador_machine(void* self);
void init_machine(Machine* machine, PhysicalMemory* pm, int num_CPUs, int num_cores_CPU, int num_threads_core);
int vaciar_cores_terminados(Machine* machine, int* pid_procesos_terminados);
void vaciar_cores_sin_saldo_suficiente(Machine* machine);
void asignar_proceso_a_machine(Machine* machine, int id_core, PCB* pcb);
int get_ids_cores_ociosos(Machine* machine, int* ids_cores_ociosos);
int get_num_cores_machine(Machine* machine);
int get_num_hilos_ociosos_machine(Machine* machine);
int get_id_primer_core_ocioso_machine(Machine* machine);

#endif //MACHINE_H
