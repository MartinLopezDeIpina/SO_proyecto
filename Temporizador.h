//
// Created by martin on 10/25/24.
//

#ifndef TEMPORIZADOR_H
#define TEMPORIZADOR_H



typedef struct {
    void* funcion;
    pthread_t thread;
} Temporizador;

void init_temporizador(Temporizador* temporizador, void* funcion, Clock* clock);


#endif //TEMPORIZADOR
