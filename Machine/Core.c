//
// Created by martin on 12/23/24.
//

#include <stdio.h>
#include <pthread.h>
#include "../PCB.h"
#include "../Boolean.h"

typedef struct {
    PCB* current_process;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    Boolean ejecutando;
}Core;

void ejecutar_instruccion(Core* core) {
    core->ejecutando = TRUE;
    printf("Ejecutando instruccion\n");
    core->ejecutando = FALSE;
}

void* funcion_core(void* arg) {
    Core* core = (Core*)arg;

    while(1) {
        pthread_mutex_lock(&core->mutex);
        pthread_cond_wait(&core->condition, &core->mutex);

        if (core->current_process && core->ejecutando != TRUE) {
            ejecutar_instruccion(core);
        }

        pthread_mutex_unlock(&core->mutex);
    }
    return NULL;
}

void init_core(Core* core) {
    core -> current_process = NULL;

    pthread_create(&core->thread, NULL, funcion_core, (void*)core);
}

void notificar_tick_clock_core(Core* core) {
    pthread_mutex_lock(&core->mutex);
    pthread_cond_signal(&core->condition);
    pthread_mutex_unlock(&core->mutex);
}


