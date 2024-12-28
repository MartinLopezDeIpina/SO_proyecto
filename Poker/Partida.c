//
// Created by martin on 12/27/24.
//
#include "Partida.h"
#include "Carta.h"
#include <stdlib.h>

#include "EvaluadorManos.h"

// Para simplificar, no se podrá apostar más que el saldo del jugador con menos saldo
void set_dinero_apuesta_maxima(Partida* partida) {
    if (partida->jugadores->cantidad == 0) {
        partida->saldo_apuesta_maxima = 0;
        return;
    }

    int min = partida->jugadores->pcbs[0]->saldo;
    for (int i = 1; i < partida->jugadores->cantidad; i++) {
        PCB* jugador = partida->jugadores->pcbs[i];
        if (jugador->saldo < min) {
            min = jugador->saldo;
        }
    }
    partida->saldo_apuesta_maxima = min;
}

void reiniciar_dinero_apostado_jugadores(Partida* partida) {
    for (int i = 0; i < partida->jugadores->cantidad; i++) {
        partida-> jugadores -> pcbs[i] -> apuesta_total_partida = 0;
    }
}

void init_partida(Partida* partida, PCBArray* candidatos, int id_core) {
    partida->baraja = (Baraja *) malloc(sizeof(Baraja));
    partida -> cartas_comunes = (Carta *) malloc(5 * sizeof(Carta));

    int num_cartas_necesarias_para_jugadores = candidatos->cantidad * 2;

    init_baraja(partida->baraja, num_cartas_necesarias_para_jugadores);
    partida->jugadores = candidatos;
    partida->id_core_en_juego = id_core;

    partida -> pot = 0;
    set_dinero_apuesta_maxima(partida);
    reiniciar_dinero_apostado_jugadores(partida);

}
void eliminar_jugador(Partida* partida, int indice) {

    // Mover los jugadores restantes una posición hacia la izquierda, sobreescribiendo al jugador eliminado
    for (int i = indice; i < partida->jugadores->cantidad - 1; i++) {
        partida->jugadores->pcbs[i] = partida->jugadores->pcbs[i + 1];
    }

    partida->jugadores->cantidad--;
}

void repartir_cartas_iniciales(Partida* partida){
    for (int i = 0; i < partida->jugadores->cantidad; i++) {
        PCB* jugador = partida->jugadores->pcbs[i];
        jugador->cartas[0] = repartir_carta(partida->baraja);
        jugador->cartas[1] = repartir_carta(partida->baraja);
    }
}

void repartir_cartas_comunes(Partida* partida){
    for (int i = 0; i < 5; i++) {
        partida->cartas_comunes[i] = repartir_carta(partida->baraja);
    }
}

void pagar_dinero_inicial(Partida* partida){
    for (int i = 0; i < partida -> jugadores ->cantidad; i++) {
        decrementar_saldo(partida -> jugadores -> pcbs[i], 1);
        partida->pot++;
    }
}

Boolean apuesta_del_jugador_iguala_sube_apuesta(int dinero_apuesta,int saldo_apostado_jugador, int saldo_apuesta_actual) {
   if(dinero_apuesta + saldo_apostado_jugador >= saldo_apuesta_actual) {
       return TRUE;
   }else {
       return FALSE;
   }
}

// para simplificar, se hace una ronda de apuestas y otra ronda para igualar o retirarse.
void jugar_ronda_apuestas(Partida* partida) {
    partida -> saldo_apuesta_actual = 0;

    //Ronda de apuestas
    for(int i = 0; i < partida->jugadores->cantidad; i++) {
        PCB* jugador = partida->jugadores->pcbs[i];
        int dinero_max_apostable_ronda = partida -> saldo_apuesta_maxima - partida -> saldo_apuesta_total;
        int dinero_apuesta = get_apuesta_ronda(jugador, partida->pot, partida -> saldo_apuesta_actual, dinero_max_apostable_ronda, partida->baraja->num_barajas, partida->cartas_comunes);

        if(apuesta_del_jugador_iguala_sube_apuesta(dinero_apuesta, jugador -> apuesta_total_partida, partida -> saldo_apuesta_actual) == TRUE){
            jugador -> apuesta_total_partida += dinero_apuesta;
            decrementar_saldo(jugador, dinero_apuesta);
            partida -> pot += dinero_apuesta;
            partida -> saldo_apuesta_total = jugador -> apuesta_total_partida;
            partida -> saldo_apuesta_actual += (dinero_apuesta - partida -> saldo_apuesta_actual);

        }else {
            eliminar_jugador(partida, i);
        }
    }

    //Ronda de igualar o retirarse
    for(int i = 0; i < partida->jugadores->cantidad; i++) {
        PCB* jugador = partida->jugadores->pcbs[i];
        int dinero_apuesta = get_apuesta_ronda(jugador, partida->pot, partida -> saldo_apuesta_actual);

        int dinero_apuesta_jugador_total = jugador -> apuesta_total_partida + dinero_apuesta;

        Boolean igualar_apuesta;
        if(dinero_apuesta_jugador_total >= partida -> saldo_apuesta_total) {
            igualar_apuesta = TRUE;
            dinero_apuesta = partida -> saldo_apuesta_total - jugador -> apuesta_total_partida;
        }else {
            igualar_apuesta = FALSE;
        }

        if(igualar_apuesta == TRUE){
            int dinero_apuesta = partida -> saldo_apuesta_total - jugador -> apuesta_total_partida;
            jugador -> apuesta_total_partida += dinero_apuesta;
            decrementar_saldo(jugador, dinero_apuesta);
            partida -> pot += dinero_apuesta;

        }else {
            eliminar_jugador(partida, i);
        }
    }
}



PCB* obtener_ganador_de_jugadores_restantes(Partida* partida) {
    PCB* ganador = NULL;
    for (int i = 0; i < partida->jugadores->cantidad; i++) {
        if (ganador == NULL) {
            ganador = partida->jugadores->pcbs[i];
        } else {
            if (mano_jugador_a_mejor_mano_jugador_b(ganador, partida -> jugadores -> pcbs[i], partida -> cartas_comunes) == FALSE) {
                ganador = partida->jugadores->pcbs[i];
            }
        }
    }
    return ganador;
}

//por ahora simplificar a dos rondas de apuestas
PCB* jugar_partida_poker(Partida* partida) {
    shuffle_baraja(partida->baraja);

    pagar_dinero_inicial(partida);

    repartir_cartas_iniciales(partida);
    jugar_ronda_apuestas(partida);

    repartir_cartas_comunes(partida);
    jugar_ronda_apuestas(partida);

    PCB* ganador = obtener_ganador_de_jugadores_restantes(partida);
    return ganador;
}
