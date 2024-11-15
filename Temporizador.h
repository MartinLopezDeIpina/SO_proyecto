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

void init_temporizador(Temporizador* temporizador, void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable, int* int_prueba);
void bucle_temporizador(Temporizador* temporizador);
void llamar_componente_temporizable(void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable);


#endif //TEMPORIZADOR
