#include <stdio.h>

#include "Clock.h"
#include "IComponenteTemporizable.h"
//
// Created by martin on 10/25/24.
//
typedef struct {
    void* componente_temporizable;
    IComponenteTemporizable i_componente_temporizable;
    Clock* clock;
} Temporizador;

void llamar_componente_temporizable(void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable) {
    i_componente_temporizable -> ejecutar_funcion_temporizador(componente_temporizable);
}

void* bucle_temporizador(void* args) {
    Temporizador* temporizador = (Temporizador*)args;
    Clock* clock = temporizador -> clock;

    //printf("Clock address: %p\n", (void*)clock);
    //printf("clock frecuencia %d\n", clock -> frecuencia);

    pthread_mutex_lock(&clock -> mutex);

    while(1) {

        int done = clock -> done;
        done++;
        clock -> done = done;

        llamar_componente_temporizable(temporizador -> componente_temporizable, &temporizador -> i_componente_temporizable);

        pthread_cond_signal(&clock -> cond1);
        pthread_cond_wait(&clock -> cond2, &clock -> mutex);
    }
}

void init_temporizador(Temporizador* temporizador, Clock* clock, void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable) {

    temporizador -> componente_temporizable = componente_temporizable;
    temporizador -> i_componente_temporizable = *i_componente_temporizable;
    temporizador -> clock = clock;

    pthread_t thread;
    //printf("Clock address before: %p\n", (void*)clock);
    //printf("clock frecuencia before %d\n", clock -> frecuencia);
    pthread_create(&thread, NULL, bucle_temporizador, temporizador);
    pthread_detach(thread);
}
