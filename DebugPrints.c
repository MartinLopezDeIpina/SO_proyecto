//
// Created by martin on 12/29/24.
//
#include <stdio.h>
#include "globals.h"
#include "PCB.h"
#include <stdarg.h>

#include "Poker/Partida.h"
#include "Poker/EvaluadorManos.h"
#include "DebugPrints.h"

static inline void debug_printf(const char* format, Boolean booleano, ...) {
    if (booleano == TRUE) {
        va_list args;
        va_start(args, format);  // Inicializar args con el último parámetro conocido
        vprintf(format, args);
        va_end(args);           // Limpiar args cuando terminemos
    }
}

void print_ronda_apuestas(Boolean preflop) {
    if(preflop == TRUE) {
        debug_printf("\n* Ronda de apuestas preflop *\n", DEBUG_PARTIDA);
    }else {
        debug_printf("\n* Ronda de apuestas flop *\n", DEBUG_PARTIDA);
    }
}
void print_jugador_apostando(PCB* jugador) {
    debug_printf("Proceso %d apostando ", DEBUG_PARTIDA, jugador->pid);
}
void print_dinero_apostado(int dinero_apuesta) {
    debug_printf("%d$\n", DEBUG_PARTIDA, dinero_apuesta);
}
void print_dinero_apostado_iguala_apuesta(int dinero_apuesta) {
    debug_printf("IGUALA APUESTA %d$\n", DEBUG_PARTIDA, dinero_apuesta);
}

void print_cartas_jugador(PCB* jugador) {
    debug_printf("Proceso %d: ", DEBUG_PARTIDA, jugador->pid);
    for (int i = 0; i < jugador->prioridad; i++) {
        debug_printf("%s ", DEBUG_PARTIDA, carta_to_string(&(jugador->cartas[i])));

        if(i <= jugador->prioridad-2) {
            debug_printf("/ ", DEBUG_PARTIDA);
        }
    }
}

void print_cartas_iniciales(Partida* partida) {
    debug_printf("\n*** Iniciando partida ***\n", DEBUG_PARTIDA);
    for (int i = 0; i < partida->jugadores->cantidad; i++) {
        PCB* jugador = partida->jugadores->pcbs[i];
        debug_printf("Proceso %d: ", DEBUG_PARTIDA, jugador->pid);
        print_cartas_jugador(jugador);
        debug_printf("\n", DEBUG_PARTIDA);
    }
}

void print_all_in() {
    debug_printf("ALL IN ", DEBUG_PARTIDA);
}

void print_info_apuesta(float equidad, float relacion) {
   debug_printf("Equidad: %2.2f% Relacion: %2.2f ", DEBUG_PARTIDA, equidad, relacion);
}

void print_eliminar_jugador_partida(int pid) {
    debug_printf("- Proceso %d eliminado de la partida -\n", DEBUG_PARTIDA, pid);
}

void print_eligiendo_ganador(Partida* partida) {
    debug_printf("\n* Eligiendo ganador *\n", DEBUG_PARTIDA);
    for(int i = 0; i < 5; i++) {
        debug_printf("%s ", DEBUG_PARTIDA, carta_to_string(&(partida->cartas_comunes[i])));

    }
    debug_printf("\n", DEBUG_PARTIDA);
    for (int i = 0; i < partida->jugadores->cantidad; i++) {
        debug_printf("Proceso %d: ", DEBUG_PARTIDA, partida->jugadores->pcbs[i]->pid);

        PCB* jugador = partida->jugadores->pcbs[i];
        print_cartas_jugador(jugador);

        Carta* todas = (Carta*) malloc(sizeof(Carta) * (jugador->prioridad+5));
        combinar_cartas(jugador->cartas, partida->cartas_comunes, jugador->prioridad, todas);

        int valor = valor_mano(todas, jugador->prioridad+5);
        debug_printf(" -> %s\n", DEBUG_PARTIDA, valor_combinacion_to_string_combinacion(valor));
    }
}

void print_ganador(Partida* partida, PCB* jugador) {
    debug_printf("Proceso %d gana la partida, se lleva %d$\n\n", DEBUG_PARTIDA, jugador->pid, partida->pot);
}

void print_proceso_num_instrucciones(int num_pags_codigo, int num_pags_data, int pid_prceso) {
    debug_printf("\n*** Reservando %d páginas de código y %d páginas de datos para el proceso %d ***\n\n", DEBUG_DIRS_MEMORIA, num_pags_codigo, num_pags_data, pid_prceso);
}

void print_direccion_escrita(uint32_t dir, uint32_t valor) {
    debug_printf("** En la dirección %X se ha escrito el valor %X **\n", DEBUG_DIRS_MEMORIA, dir, valor);
}

void print_error_direccion_no_alineada(uint32_t dir) {
    debug_printf("Error: la dirección %X no está alineada a 4 bytes\n", DEBUG_DIRS_MEMORIA, dir);
}

void print_ejecutando_instruccion(int pid, uint32_t instruccion) {
    debug_printf("Proceso %d ejecutando instrucción %X\n", DEBUG_DIRS_MEMORIA, pid, instruccion);
}
void print_instruccion_ld(int pid, int num_registro, uint32_t valor) {
    debug_printf("Proceso %d ejecutando instrucción LD en registro %d con valor %X\n", DEBUG_DIRS_MEMORIA, pid, num_registro, valor);
}
void print_instruccion_st(int pid, int dir_fisica, uint32_t valor) {
    debug_printf("Proceso %d ejecutando instrucción ST en dir física %X con valor %X\n", DEBUG_DIRS_MEMORIA, pid, dir_fisica, valor);
}
void print_instruccion_add(int pid, uint32_t valor1, uint32_t valor2, uint32_t resultado) {
    debug_printf("Proceso %d ejecutando instrucción ADD con valores %X y %X, resultado: %X\n", DEBUG_DIRS_MEMORIA, pid, valor1, valor2, resultado);
}
void print_instruccion_exit(int pid) {
    debug_printf("Proceso %d ejecutando instrucción EXIT\n", DEBUG_DIRS_MEMORIA, pid);
}

