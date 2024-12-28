//
// Created by martin on 12/27/24.
//

#ifndef PARTIDA_H
#define PARTIDA_H
#include "Baraja.h"
#include "../PCB.h"

#endif //PARTIDA_H

typedef struct {
   Baraja* baraja;
   PCBArray* jugadores;
   Carta* cartas_comunes;
   int id_core_en_juego;
   int saldo_apuesta_actual;
   int saldo_apuesta_total;
   int saldo_apuesta_maxima;
   int pot;
}Partida;

void init_partida(Partida* partida, PCBArray* candidatos, int id_core);
PCB* jugar_partida_poker(Partida* partida);
