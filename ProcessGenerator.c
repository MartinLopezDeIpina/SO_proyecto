//
// Created by martin on 10/29/24.
//

#include <stdio.h>
#include "IComponenteTemporizable.h"

typedef struct {
    IComponenteTemporizable* componente_temporizable;
    int frecuencia;
}ProcessGenerator;

void init_process_generator(ProcessGenerator* process_generator, int frecuencia) {
    process_generator -> frecuencia = frecuencia;
}

void generar_proceso(ProcessGenerator* process_generator) {
    printf("Generando proceso\n");
}

void ejecutar_funcion_temporizador(void* self) {
    ProcessGenerator* process_generator = (ProcessGenerator*)self;
    generar_proceso(process_generator);
}
