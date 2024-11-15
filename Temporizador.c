#include <stdio.h>

#include "Clock.h"
#include "IComponenteTemporizable.h"
//
// Created by martin on 10/25/24.
//

pthread_mutex_t mutex_temp = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    void* componente_temporizable;
    IComponenteTemporizable i_componente_temporizable;
    int* int_prueba;
} Temporizador;

void llamar_componente_temporizable(void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable) {
    i_componente_temporizable -> ejecutar_funcion_temporizador(componente_temporizable);
}

void bucle_temporizador(Temporizador* temporizador) {
    Clock* clock = get_global_clock();

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

void init_temporizador(Temporizador* temporizador, void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable, int* int_prueba) {

    temporizador -> componente_temporizable = componente_temporizable;
    temporizador -> i_componente_temporizable = *i_componente_temporizable;
    temporizador -> int_prueba = int_prueba;

    pthread_t thread;
    pthread_create(&thread, NULL, bucle_temporizador, temporizador);
    pthread_detach(thread);
}
