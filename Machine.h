//
// Created by martin on 11/3/24.
//

#ifndef MACHINE_H
#define MACHINE_H

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    int num_CPUs;
    int num_cores_CPU;
    int num_threads_core;
}Machine;

void funcion_machine(Machine* machine);
void ejecutar_funcion_temporizador_machine(void* self);
void init_machine(Machine* machine, int num_CPUs, int num_cores_CPU, int num_threads_core);

#endif //MACHINE_H
