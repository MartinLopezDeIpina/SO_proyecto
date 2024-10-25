#include <stdio.h>

#include "Clock.h"
//
// Created by martin on 10/25/24.
//
typedef struct {
    void (*funcion)(void);
    Clock* clock;
} Temporizador;

void* bucle_temporizador(void* args) {
    Temporizador* temporizador = (Temporizador*)args;
    Clock* clock = temporizador -> clock;

    pthread_mutex_lock(&clock -> mutex);

    while(1) {

        int done = clock -> done;
        done++;
        clock -> done = done;

        temporizador -> funcion();

        pthread_cond_signal(&clock -> cond1);
        pthread_cond_wait(&clock -> cond2, &clock -> mutex);
    }
}

void init_temporizador(Temporizador* temporizador, void (*funcion)(void), Clock* clock) {
    temporizador -> funcion = funcion;
    temporizador -> clock = clock;

    pthread_t thread;
    pthread_create(&thread, NULL, bucle_temporizador, temporizador);
}
