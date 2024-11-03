//
// Created by martin on 11/3/24.
//

#include <stdio.h>

#include "IComponenteTemporizable.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    int num_CPUs;
}Machine;

void funcion_machine(Machine* machine) {
    printf("ejeutando funcion machine\n");
}

void ejecutar_funcion_temporizador_machine(void* self) {
    Machine* machine = (Machine*)self;
    funcion_machine(machine);
}

void init_machine(Machine* machine, int num_CPUs) {
    machine -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_machine;
    machine -> num_CPUs = num_CPUs;
}
