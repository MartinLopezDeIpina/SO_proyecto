//
// Created by martin on 10/29/24.
//

#include <pthread.h>
#include <stdio.h>
#include <bits/pthreadtypes.h>
#include "PCB.h"

#include <stdlib.h>

#include "utils.h"
#include "Poker/PokerUtils.h"

#include "Boolean.h"


void init_pcb(PCB* pcb, int pid, int prioridad) {
    pcb -> pid = pid;
    pcb -> prioridad = prioridad;
    pcb -> num_instruccion_actual = 0;
    pcb -> num_instrucciones = 5;
    pcb -> estado = LISTO;

    pcb -> saldo = 5;
    pcb -> saldo_ejecucion = 0;
    pcb -> agresividad = float_aleatorio_entre_dos_numeros(0.0f, 0.5f);
    pcb -> min_saldo_entrar_core = 3;
    pcb -> cartas = (Carta*)malloc(sizeof(Carta) * 2);

    pthread_mutex_init(&pcb -> mutex, NULL);
}

void ejecutar_instruccion_proceso(PCB* pcb) {
    if (pcb == NULL) {
        return;
    }

    pthread_mutex_lock(&pcb -> mutex);

    if (pcb -> num_instruccion_actual >= pcb -> num_instrucciones) {
        pcb -> estado = TERMINADO;
    }else {
        pcb -> num_instruccion_actual++;
        decrementar_saldo_ejecucion(pcb, 1);
    }

    pthread_mutex_unlock(&pcb -> mutex);
}

Boolean proceso_esta_en_estado(PCB* pcb, EstadoProceso estado) {
    if (pcb == NULL) {
        return FALSE;
    }

    pthread_mutex_lock(&pcb -> mutex);

    Boolean esta_en_estado = FALSE;

    if (pcb -> estado == estado) {
        esta_en_estado = TRUE;
    }

    pthread_mutex_unlock(&pcb -> mutex);

    return esta_en_estado;
}

Boolean proceso_ha_terminado(PCB* pcb) {
    return proceso_esta_en_estado(pcb, TERMINADO);
}
Boolean proceso_esta_listo(PCB* pcb) {
    return proceso_esta_en_estado(pcb, LISTO);
}
Boolean proceso_saldo_ejecucion_insuficiente(PCB* pcb) {
    return proceso_esta_en_estado(pcb, SALDO_INSUFICIENTE);
}

Boolean proceso_saldo_suficiente_para_entrar_core(PCB* pcb) {
    pthread_mutex_lock(&pcb -> mutex);
    Boolean saldo_suficiente = FALSE;
    if (pcb -> saldo > 0) {
        saldo_suficiente = TRUE;
    }
    pthread_mutex_unlock(&pcb -> mutex);

    return saldo_suficiente;
}

void asignar_saldo_ejecucion(PCB* pcb) {
    pthread_mutex_lock(&pcb -> mutex);
    int saldo_a_asignar;
    if(pcb -> saldo <= 1) {
        saldo_a_asignar = pcb -> saldo;
    }else {
        saldo_a_asignar = (pcb -> saldo + 1) / 2;
    }
    pcb -> saldo -= saldo_a_asignar;
    pcb -> saldo_ejecucion = saldo_a_asignar;
    pthread_mutex_unlock(&pcb -> mutex);
}

void set_estado_proceso(PCB* pcb, EstadoProceso estado) {
    pthread_mutex_lock(&pcb -> mutex);
    pcb -> estado = estado;
    pthread_mutex_unlock(&pcb -> mutex);
}

void set_estado_proceso_ejecutando(PCB* pcb) {
    set_estado_proceso(pcb, EJECUTANDO);
}
void set_estado_proceso_listo(PCB* pcb) {
    set_estado_proceso(pcb, LISTO);
}

void printear_instrucciones_ejecutadas(PCB* pcb) {
    pthread_mutex_lock(&pcb -> mutex);

    int instruccion_actual = pcb -> num_instruccion_actual;
    int num_instrucciones = pcb -> num_instrucciones;

    printf("Proceso %d [", pcb -> pid);

    for (int i = 0; i < instruccion_actual; i++) {
        printf("*");
    }

    for (int i = instruccion_actual; i < num_instrucciones; i++) {
        printf(" ");
    }

    printf("] %d/%d", instruccion_actual, num_instrucciones);

    printf(" %d$\n", pcb -> saldo_ejecucion);

    pthread_mutex_unlock(&pcb -> mutex);
}

void incrementar_saldo(PCB* pcb, int cantidad) {
    pthread_mutex_lock(&pcb -> mutex);
    pcb -> saldo += cantidad;
    pthread_mutex_unlock(&pcb -> mutex);
}
void decrementar_saldo(PCB* pcb, int cantidad) {
    pthread_mutex_lock(&pcb -> mutex);
    pcb -> saldo -= cantidad;
    pthread_mutex_unlock(&pcb -> mutex);
}
void incrementar_saldo_ejecucion(PCB* pcb, int cantidad) {
    pthread_mutex_lock(&pcb -> mutex);
    pcb -> saldo_ejecucion += cantidad;
    pthread_mutex_unlock(&pcb -> mutex);
}

// No poner mutex porque como va dentro de ejecutar instrucción ya tiene el mutex
void decrementar_saldo_ejecucion(PCB* pcb, int cantidad) {
    pcb -> saldo_ejecucion -= cantidad;
    if (pcb -> saldo_ejecucion <= 0) {
        pcb -> estado = SALDO_INSUFICIENTE;
    }
}


int get_apuesta_ronda_pcb(PCB* pcb, Boolean preflop, Carta* cartas_conocidas,int num_jugadores, int pot, int saldo_apuesta_actual) {
    Carta* cartas;
    int num_cartas;
    // Juntar cartas de la mano con las de la mesa
    if (preflop == FALSE) {
        Carta* cartas = malloc(sizeof(Carta) * pcb -> prioridad + 5);
        num_cartas = pcb -> prioridad + 5;
        for (int i = 0; i < pcb -> prioridad; i++) {
           cartas[i] = pcb -> cartas[i];
        }
        for (int i = 0; i < 5; i++) {
            cartas[pcb -> prioridad + i] = cartas_conocidas[i];
        }
    }else {
       Carta* cartas = malloc(sizeof(Carta) * pcb -> prioridad);
       num_cartas = pcb -> prioridad;
       for (int i = 0; i < pcb -> prioridad; i++) {
           cartas[i] = pcb -> cartas[i];
       }
    }
    // Hacer un farol con la probabilidad de la agresividad
    if(evento_con_probabilidad(pcb -> agresividad)) {
        return pcb -> saldo;
    }
    int dinero_a_apostar = get_dinero_a_apostar(cartas, num_cartas, pot, saldo_apuesta_actual, num_jugadores, preflop);
    // No apostar más de lo que se tiene
    if(dinero_a_apostar >= pcb -> saldo) {
        dinero_a_apostar = pcb -> saldo;
    }
    return dinero_a_apostar;
}