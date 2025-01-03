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
void print_eligiendo_ganador(Partida* partida);
void print_ganador(Partida* partida, PCB* jugador);

void print_proceso_num_instrucciones(int num_instrucciones_codigo, int num_instrucciones_data, int pid_prceso);
void print_direccion_escrita(uint32_t dir, uint32_t valor);
void print_error_direccion_no_alineada(uint32_t dir);

#endif //DEBUGPRINTS_H
