//
// Created by martin on 10/25/24.
//

#ifndef TEMPORIZADOR_H
#define TEMPORIZADOR_H
#include "IComponenteTemporizable.h"


typedef struct {
    void* componente_temporizable;
    IComponenteTemporizable i_componente_temporizable;
    float frecuencia;
    float resto_tick;
} Temporizador;

void init_temporizador(Temporizador* temporizador, void* componente_temporizable, IComponenteTemporizable* i_componente_temporizable, float frecuencia);
void bucle_temporizador(Temporizador* temporizador);
void llamar_componente_temporizable(Temporizador* temporizador);


#endif //TEMPORIZADOR
