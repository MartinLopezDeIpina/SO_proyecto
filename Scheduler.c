//
// Created by martin on 11/3/24.
//

#include <stdio.h>

#include "IComponenteTemporizable.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
}Scheduler;

void funcion_scheduler(Scheduler* scheduler) {
    printf("ejeutando funcion scheduler\n");
}

void ejecutar_funcion_temporizador_scheduler(void* self) {
    Scheduler* scheduler = (Scheduler*)self;
    funcion_scheduler(scheduler);
}

void init_scheduler(Scheduler* scheduler) {
    scheduler -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_scheduler;
}
