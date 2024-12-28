//
// Created by martin on 12/27/24.
//

#ifndef BARAJA_H
#define BARAJA_H
#include "Carta.h"

#endif //BARAJA_H

typedef struct {
    Carta* cartas;
    int num_cartas;
    int num_barajas;
}Baraja;

void init_baraja(Baraja* baraja, int num_cartas_necesarias_para_jugadores);
void shuffle_baraja(Baraja* baraja);
void print_baraja(Baraja* baraja);

Carta repartir_carta(Baraja* baraja);

