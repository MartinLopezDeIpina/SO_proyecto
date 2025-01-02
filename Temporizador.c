#include <stdio.h>

#include "Clock.h"
#include "IComponenteTemporizable.h"
#include "Loader.h"
//
// Created by martin on 10/25/24.
//

pthread_mutex_t mutex_temp = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    void* componente_temporizable;
    IComponenteTemporizable i_componente_temporizable;
    float frecuencia;
    float resto_tick;
} Temporizador;


/**
 * Debe ejecutar la función del componente frecuencia veces por cada tick de reloj.
 * Por ejemplo, el temporizador del generador de procesos podría tener una frecuencia de 0.5, llamándolo cada 2 ticks de reloj, creando un proceso cada 2 ticks.
 */
void llamar_componente_temporizable(Temporizador* temporizador) {
    float* resto_tick = &temporizador -> resto_tick;
    IComponenteTemporizable* i_componente_temporizable = &temporizador -> i_componente_temporizable;
    void* componente_temporizable = temporizador -> componente_temporizable;

    *resto_tick += temporizador -> frecuencia;

    while(*resto_tick >= 1) {
        i_componente_temporizable -> ejecutar_funcion_temporizador(componente_temporizable);
        *resto_tick -= 1;
    }
}

void bucle_temporizador(Temporizador* temporizador) {
    Clock* clock = get_global_clock();

    pthread_mutex_lock(&clock -> mutex);

    while(1) {
        int done = clock -> done;
        done++;
        clock -> done = done;

        llamar_componente_temporizable(temporizador);

        pthread_cond_signal(&clock -> cond1);
        pthread_cond_wait(&clock -> cond2, &clock -> mutex);
    }
}

void init_temporizador(Temporizador* temporizador, void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable, float frecuencia) {

    temporizador -> componente_temporizable = componente_temporizable;
    temporizador -> i_componente_temporizable = *i_componente_temporizable;
    temporizador -> frecuencia = frecuencia;
    temporizador -> resto_tick = 0;

    pthread_t thread;
    pthread_create(&thread, NULL, bucle_temporizador, temporizador);
    //pthread_detach(thread);
}
