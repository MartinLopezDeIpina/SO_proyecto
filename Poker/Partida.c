//
// Created by martin on 12/27/24.
//
#include "Partida.h"
#include "Carta.h"
#include <stdlib.h>
#include "../DebugPrints.h"

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

    int num_cartas_necesarias_para_jugadores = candidatos->cantidad * 3;

    init_baraja(partida->baraja, num_cartas_necesarias_para_jugadores);
    partida->jugadores = candidatos;
    partida->id_core_en_juego = id_core;

    partida -> pot = 0;
    set_dinero_apuesta_maxima(partida);
    reiniciar_dinero_apostado_jugadores(partida);

    partida -> saldo_apuesta_total = 0;
    partida -> saldo_apuesta_inicial = 1;
}
void eliminar_jugador(Partida* partida, int pid) {
    print_eliminar_jugador_partida(pid);

    int indice = -1;
    for(int i = 0; i < partida->jugadores_aux->cantidad; i++) {
        if(partida->jugadores_aux->pcbs[i]->pid == pid) {
            indice = i;
            break;
        }
    }

    // Mover los jugadores restantes una posición hacia la izquierda, sobreescribiendo al jugador eliminado
    for (int i = indice; i < partida->jugadores_aux->cantidad - 1; i++) {
        partida->jugadores_aux->pcbs[i] = partida->jugadores_aux->pcbs[i + 1];
    }

    partida->jugadores_aux->cantidad--;
}


void repartir_cartas_iniciales(Partida* partida){
    for (int i = 0; i < partida->jugadores->cantidad; i++) {
        PCB* jugador = partida->jugadores->pcbs[i];
        for(int j = 0; j < jugador->prioridad; j++) {
            jugador->cartas[j] = repartir_carta(partida->baraja);
        }
    }
    print_cartas_iniciales(partida);
}

void repartir_cartas_comunes(Partida* partida){
    for (int i = 0; i < 5; i++) {
        partida->cartas_comunes[i] = repartir_carta(partida->baraja);
    }
}

void pagar_dinero_inicial(Partida* partida){
    for (int i = 0; i < partida -> jugadores ->cantidad; i++) {
        decrementar_saldo(partida -> jugadores -> pcbs[i], 1);
        partida -> jugadores -> pcbs[i] -> apuesta_total_partida = 1;
        partida->pot++;
    }
    partida -> saldo_apuesta_total = 1;
}

Boolean apuesta_del_jugador_iguala_sube_apuesta(int dinero_apuesta,int saldo_apostado_jugador, int saldo_apuesta_actual) {
   if(dinero_apuesta + saldo_apostado_jugador >= saldo_apuesta_actual) {
       return TRUE;
   }else {
       return FALSE;
   }
}

void jugar_ronda_inicial_de_apuestas(Partida* partida, Boolean preflop) {
    print_ronda_apuestas(preflop);

   //Si solo queda un jugador, no tiene sentido jugar
    if (partida->jugadores->cantidad <= 1) {
        return;
    }

    for(int i = 0; i < partida->jugadores->cantidad; i++) {
        print_jugador_apostando(partida->jugadores->pcbs[i]);

        PCB* jugador = partida->jugadores->pcbs[i];
        int dinero_apuesta = get_apuesta_ronda_pcb(jugador, partida->id_core_en_juego, preflop, partida->cartas_comunes, partida->jugadores->cantidad, partida->pot, partida->saldo_apuesta_total, FALSE);

        int dinero_apuesta_total_jugador = jugador -> apuesta_total_partida + dinero_apuesta;
        if(dinero_apuesta_total_jugador >= partida -> saldo_apuesta_maxima) {
            dinero_apuesta = partida -> saldo_apuesta_maxima - jugador -> apuesta_total_partida;
        }

        print_dinero_apostado(dinero_apuesta);
        if(dinero_apuesta + jugador->apuesta_total_partida >= partida->saldo_apuesta_total){
            jugador -> apuesta_total_partida += dinero_apuesta;
            decrementar_saldo(jugador, dinero_apuesta);
            partida -> pot += dinero_apuesta;
            partida -> saldo_apuesta_total = jugador -> apuesta_total_partida;

        }else {
            eliminar_jugador(partida, jugador->pid);
        }
    }
}

void jugar_ronda_de_igualar_o_retirarse(Partida* partida, Boolean preflop) {
    print_ronda_apuestas(preflop);

   //Si solo queda un jugador, no tiene sentido jugar
    if (partida->jugadores->cantidad <= 1) {
        return;
    }

    for(int i = 0; i < partida->jugadores->cantidad; i++) {
        print_jugador_apostando(partida->jugadores->pcbs[i]);

        PCB* jugador = partida->jugadores->pcbs[i];

        int dinero_apuesta_necesario_jugador = partida -> saldo_apuesta_total - jugador -> apuesta_total_partida;

        int dinero_apuesta = get_apuesta_ronda_pcb(jugador, partida->id_core_en_juego, preflop, partida -> cartas_comunes, partida->jugadores->cantidad, partida -> pot, partida->saldo_apuesta_total, TRUE);

        int dinero_apuesta_jugador_total = jugador -> apuesta_total_partida + dinero_apuesta;

        Boolean igualar_apuesta;
        if(dinero_apuesta_jugador_total >= partida -> saldo_apuesta_total) {
            igualar_apuesta = TRUE;
            dinero_apuesta = partida -> saldo_apuesta_total - jugador -> apuesta_total_partida;
            print_dinero_apostado_iguala_apuesta(dinero_apuesta);
        }else {
            igualar_apuesta = FALSE;
        }

        if(igualar_apuesta == TRUE){
            jugador -> apuesta_total_partida += dinero_apuesta;
            decrementar_saldo(jugador, dinero_apuesta);
            partida -> pot += dinero_apuesta;
        }else {
            eliminar_jugador(partida, jugador->pid);
        }
    }
}


// para simplificar, se hace una ronda de apuestas y otra ronda para igualar o retirarse.
void jugar_ronda_apuestas(Partida* partida, Boolean preflop) {
    // Crear copia de jugadores para ir eliminnando los que se retiran y poder seguir iterando sin problemas
    partida -> jugadores_aux = shallow_copy_pcb_array(partida -> jugadores);

    jugar_ronda_inicial_de_apuestas(partida, preflop);

    free(partida -> jugadores);
    partida -> jugadores = shallow_copy_pcb_array(partida -> jugadores_aux);

    jugar_ronda_de_igualar_o_retirarse(partida, preflop);

    free(partida -> jugadores);
    partida -> jugadores = shallow_copy_pcb_array(partida -> jugadores_aux);
    free(partida -> jugadores_aux);
}

PCB* obtener_ganador_de_jugadores_restantes(Partida* partida) {
    print_eligiendo_ganador(partida);
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
    print_ganador(partida, ganador);
    return ganador;
}

//por ahora simplificar a dos rondas de apuestas
PCB* jugar_partida_poker(Partida* partida) {
    shuffle_baraja(partida->baraja);

    pagar_dinero_inicial(partida);

    repartir_cartas_iniciales(partida);
    jugar_ronda_apuestas(partida, TRUE);

    repartir_cartas_comunes(partida);
    jugar_ronda_apuestas(partida, FALSE);

    PCB* ganador = obtener_ganador_de_jugadores_restantes(partida);
    incrementar_saldo(ganador, partida->pot);
    return ganador;
}
