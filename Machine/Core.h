//
// Created by martin on 12/23/24.
//

#ifndef CORE_H
#define CORE_H

#endif //CORE_H
#include <pthread.h>
#include "../PCB.h"
#include "../Boolean.h"

typedef struct {
    int id_core;
    PCB* current_process;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    Boolean ejecutando;
}Core;

void init_core(int id_core, Core* core);

void ejecutar_instruccion(Core* core);

void *funcion_core(void* arg);

void notificar_tick_clock_core(Core* core);

Boolean core_esta_ocioso(Core* core);
