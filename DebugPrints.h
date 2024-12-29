//
// Created by martin on 12/29/24.
//

#ifndef DEBUGPRINTS_H
#define DEBUGPRINTS_H
#include "Poker/Partida.h"


void print_ronda_apuestas_preflop();
void print_ronda_apuestas_flop();
void print_jugador_apostando(PCB* jugador);
void print_dinero_apostado(int dinero_apuesta);
void print_dinero_apostado_iguala_apuesta(int dinero_apuesta);
void print_cartas_iniciales(Partida* partida);
void print_all_in();
void print_info_apuesta(float equidad, float relacion);
void print_eliminar_jugador_partida(int pid);
void print_ronda_apuestas(Boolean preflop);

#endif //DEBUGPRINTS_H
