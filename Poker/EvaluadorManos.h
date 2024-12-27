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


Boolean mano_jugador_a_mejor_mano_jugador_b(PCB* a, PCB* b, Carta* cartas_comunes);

#endif //EVALUADORMANOS_H
