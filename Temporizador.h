//
// Created by martin on 10/25/24.
//

#ifndef TEMPORIZADOR_H
#define TEMPORIZADOR_H
#include "IComponenteTemporizable.h"


typedef struct {
    void* funcion;
    pthread_t thread;
} Temporizador;

void init_temporizador(Temporizador* temporizador, Clock* clock, void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable);


#endif //TEMPORIZADOR
