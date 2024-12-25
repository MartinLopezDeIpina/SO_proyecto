//
// Created by martin on 10/29/24.
//

#include <pthread.h>
#include <stdio.h>
#include <bits/pthreadtypes.h>

#include "Boolean.h"
typedef enum {
    LISTO,
    EJECUTANDO,
    ESPERANDO,
    TERMINADO
} EstadoProceso;

typedef struct {
    int pid;
    int num_instruccion_actual;
    int num_instrucciones;
    EstadoProceso estado;
    pthread_mutex_t mutex;
} PCB;

void init_pcb(PCB* pcb, int pid) {
    pcb -> pid = pid;
    pcb -> num_instruccion_actual = 0;
    pcb -> num_instrucciones = 5;
    pcb -> estado = LISTO;
    pthread_mutex_init(&pcb -> mutex, NULL);
}

void ejecutar_instruccion_proceso(PCB* pcb) {
    if (pcb == NULL) {
        return;
    }

    pthread_mutex_lock(&pcb -> mutex);

    if (pcb -> num_instruccion_actual >= pcb -> num_instrucciones) {
        pcb -> estado = TERMINADO;
    }else {
        pcb -> num_instruccion_actual++;
    }

    pthread_mutex_unlock(&pcb -> mutex);
}

Boolean proceso_ha_terminado(PCB* pcb) {
    if(pcb == NULL) {
        return FALSE;
    }

    pthread_mutex_lock(&pcb -> mutex);

    Boolean terminado = FALSE;

    if (pcb -> estado == TERMINADO) {
        terminado = TRUE;
    }

    pthread_mutex_unlock(&pcb -> mutex);

    return terminado;
}

void set_estado_proceso_ejecutando(PCB* pcb) {
    pthread_mutex_lock(&pcb -> mutex);
    pcb -> estado = EJECUTANDO;
    pthread_mutex_unlock(&pcb -> mutex);
}

void printear_instrucciones_ejecutadas(PCB* pcb) {
    pthread_mutex_lock(&pcb -> mutex);

    int instruccion_actual = pcb -> num_instruccion_actual;
    int num_instrucciones = pcb -> num_instrucciones;

    printf("Proceso %d [", pcb -> pid);

    for (int i = 0; i < instruccion_actual; i++) {
        printf("*");
    }

    for (int i = instruccion_actual; i < num_instrucciones; i++) {
        printf(" ");
    }

    printf("] %d/%d instrucciones\n", instruccion_actual, num_instrucciones);

    pthread_mutex_unlock(&pcb -> mutex);
}