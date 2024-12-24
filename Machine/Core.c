//
// Created by martin on 12/23/24.
//

#include <stdio.h>
#include <pthread.h>
#include "../PCB.h"
#include "../Boolean.h"

typedef struct {
    int id_core;
    PCB* current_process;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
}Core;

void ejecutar_instruccion(Core* core) {
    ejecutar_instruccion_proceso(core -> current_process);
}

void* funcion_core(void* arg) {
    Core* core = (Core*)arg;

    while(1) {
        pthread_mutex_lock(&core->mutex);
        pthread_cond_wait(&core->condition, &core->mutex);

        if (core->current_process) {
            ejecutar_instruccion(core);
        }

        pthread_mutex_unlock(&core->mutex);
    }
    return NULL;
}

void init_core(int id_core, Core* core) {
    core -> current_process = NULL;
    core -> id_core = id_core;

    pthread_create(&core->thread, NULL, funcion_core, (void*)core);
}

void notificar_tick_clock_core(Core* core) {
    pthread_mutex_lock(&core->mutex);
    pthread_cond_signal(&core->condition);
    pthread_mutex_unlock(&core->mutex);
}

Boolean core_esta_ocioso(Core* core) {
    if (core -> current_process == NULL) {
        return TRUE;
    }
    return proceso_ejecutando_ultima_instruccion(core -> current_process);
}


