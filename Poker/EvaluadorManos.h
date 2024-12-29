//
// Created by martin on 12/27/24.
//

#ifndef EVALUADORMANOS_H
#define EVALUADORMANOS_H


#include <stdio.h>
#include <stdlib.h>
#include "Carta.h"
#include "../Boolean.h"
#include "../PCB.h"


int valor_mano(Carta* cartas, int size);
Boolean mano_jugador_a_mejor_mano_jugador_b(PCB* a, PCB* b, Carta* cartas_comunes);
Boolean mano_a_mejor_mano_b(Carta* cartas_a, Carta* cartas_b, int mano_size_a, int mano_size_b, Carta* cartas_comunes);
void combinar_cartas(Carta* propias, Carta* comunes, int mano_size, Carta* resultado);

#endif //EVALUADORMANOS_H
