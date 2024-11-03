//
// Created by martin on 10/25/24.
//

#ifndef RELOJ_H
#define RELOJ_H
#include <pthread.h>

typedef struct{
    int frecuencia;
    int done;
    int num_temps;
    pthread_mutex_t mutex;
    pthread_cond_t cond1;
    pthread_cond_t cond2;
}Clock;

void clock_mandar_tick(Clock* clock);
void bucle_clock(Clock* clock);
void init_clock(Clock* clock, int frecuencia, int num_temps);
void sleep_tiempo_espera(int frecuencia);

#endif //RELOJ_H
