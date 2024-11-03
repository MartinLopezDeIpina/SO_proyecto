//
// Created by martin on 10/22/24.
//

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    int frecuencia;
    int done;
    int num_temps;
    pthread_mutex_t mutex;
    pthread_cond_t cond1;
    pthread_cond_t cond2;

} Clock;

void sleep_tiempo_espera(int frecuencia) {
    float tiempo_espera_segundos = 1.0 / frecuencia;
    usleep(tiempo_espera_segundos * 1000000);
}

void clock_mandar_tick(Clock* clock) {
    pthread_mutex_lock(&clock -> mutex);

    while (clock -> done < clock -> num_temps) {
        pthread_cond_wait(&clock -> cond1, &clock -> mutex);
    }
    clock -> done = 0;

    pthread_cond_broadcast(&clock -> cond2);
    pthread_mutex_unlock(&clock -> mutex);
}

void* bucle_clock(void* arg) {
    Clock* clock = (Clock*)arg;

    // Esperar la frecuencia porque los temporizadores se inician por defecto, para que no se ejecuten dos veces
    sleep_tiempo_espera(clock -> frecuencia);

    while (1) {
        clock_mandar_tick(clock);
        sleep_tiempo_espera(clock -> frecuencia);
    }
    return NULL;
}

void init_clock(Clock* clock, int frecuencia, int num_temps) {
    clock -> frecuencia = frecuencia;
    clock -> done = 0;
    clock -> num_temps = num_temps;

    pthread_mutex_init(&clock -> mutex, NULL);
    pthread_cond_init(&clock -> cond1, NULL);
    pthread_cond_init(&clock -> cond2, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, bucle_clock, clock);
}

