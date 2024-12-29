//
// Created by martin on 12/29/24.
//
#include <stdio.h>
#include "globals.h"
#include "PCB.h"
#include <stdarg.h>

#include "Poker/Partida.h"

static inline void debug_printf(const char* format, ...) {
    if (debug_partida == TRUE) {
        va_list args;
        va_start(args, format);  // Inicializar args con el último parámetro conocido
        vprintf(format, args);
        va_end(args);           // Limpiar args cuando terminemos
    }
}

void print_ronda_apuestas(Boolean preflop) {
    if(preflop == TRUE) {
        debug_printf("\n* Ronda de apuestas preflop *\n");
    }else {
        debug_printf("\n* Ronda de apuestas flop *\n");
    }
}
void print_jugador_apostando(PCB* jugador) {
    debug_printf("Proceso %d apostando ", jugador->pid);
}
void print_dinero_apostado(int dinero_apuesta) {
    debug_printf("%d$\n", dinero_apuesta);
}
void print_dinero_apostado_iguala_apuesta(int dinero_apuesta) {
    debug_printf("IGUALA APUESTA %d$\n", dinero_apuesta);
}
void print_cartas_iniciales(Partida* partida) {
    if(debug_partida == TRUE) {
        debug_printf("*** Iniciando partida ***\n");
        for (int i = 0; i < partida->jugadores->cantidad; i++) {
            debug_printf("Proceso %d: ", partida->jugadores->pcbs[i]->pid);
            debug_printf("%s / %s \n", carta_to_string(&(partida->jugadores->pcbs[i]->cartas[0])), carta_to_string(&(partida->jugadores->pcbs[i]->cartas[1])));
        }
    }
}

void print_all_in() {
    debug_printf("ALL IN ");
}

void print_info_apuesta(float equidad, float relacion) {
   debug_printf("Equidad: %2.2f% Relacion: %2.2f ", equidad, relacion);
}

void print_eliminar_jugador_partida(int pid) {
    debug_printf("- Proceso %d eliminado de la partida -\n", pid);
}

