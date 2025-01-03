//
// Created by martin on 10/29/24.
//

#include <pthread.h>
#include <stdio.h>
#include <bits/pthreadtypes.h>
#include "PCB.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "Poker/PokerUtils.h"

#include "Boolean.h"
#include "DebugPrints.h"
#include "globals.h"
#include "Machine/HiloHardware.h"
#include "Machine/MMU.h"


void init_estado_ejecucion_proceso(EstadoEjecucionProceso* estado_ejecucion_proceso) {
    estado_ejecucion_proceso -> PC = 0;
    estado_ejecucion_proceso -> IR = 0;
    for (int i = 0; i < 16; i++) {
        estado_ejecucion_proceso -> registros[i] = 0;
    }
}

void init_mm(mm* mm_instancia) {
    mm_instancia->code = (uint32_t*)malloc(sizeof(uint32_t));
    mm_instancia->data = (uint32_t*)malloc(sizeof(uint32_t));
    mm_instancia->pgb = (uint32_t*)malloc(sizeof(uint32_t));

    *(mm_instancia->code) = 0;
    *(mm_instancia->data) = 0;
    *(mm_instancia->pgb) = 0;
}


void init_pcb(PCB* pcb, int pid, int prioridad) {
    pcb -> pid = pid;
    pcb -> prioridad = prioridad;
    pcb -> num_instruccion_actual = 0;
    pcb -> estado = LISTO;
    pcb -> indice_ultimo_core_visitado = -1;

    pcb -> saldo = 12;
    pcb -> saldo_ejecucion = 0;
    pcb -> agresividad = float_aleatorio_entre_dos_numeros(0.0f, 0.25f);
    pcb -> min_saldo_entrar_core = 3;
    pcb -> cartas = (Carta*)malloc(sizeof(Carta) * prioridad);

    mm* mm_pcb = (mm*)malloc(sizeof(mm));
    init_mm(mm_pcb);
    pcb->mm_pcb = mm_pcb;


    EstadoEjecucionProceso* estado_ejecucion_proceso = (EstadoEjecucionProceso*)malloc(sizeof(EstadoEjecucionProceso));
    init_estado_ejecucion_proceso(estado_ejecucion_proceso);
    pcb->estado_ejecucion_proceso = estado_ejecucion_proceso;

    pthread_mutex_init(&pcb -> mutex, NULL);
}

/*
 * Contiene la lógica del número de instrucciones ejecutados y el saldo del proceso.
 */
void avanzar_ejecucion_proceso(PCB* pcb) {
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
    if (pcb -> saldo > SALDO_NECESARIO_PARA_JUGAR_POKER) {
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
void set_estado_proceso_terminado(PCB* pcb) {
    set_estado_proceso(pcb, TERMINADO);
}

char* printear_instrucciones_ejecutadas(PCB* pcb) {
    char* resultado = malloc(200);
    pthread_mutex_lock(&pcb->mutex);

    int instruccion_actual = pcb->num_instruccion_actual;
    int num_instrucciones = pcb->num_instrucciones;

    sprintf(resultado, "Proceso %d [", pcb->pid);

    char temp[200];
    temp[0] = '\0';

    for (int i = 0; i < instruccion_actual; i++) {
        strcat(temp, "*");
    }
    for (int i = instruccion_actual; i < num_instrucciones; i++) {
        strcat(temp, " ");
    }

    char final[100];
    sprintf(final, "] %d/%d %d$\n", instruccion_actual, num_instrucciones, pcb->saldo_ejecucion);

    strcat(resultado, temp);
    strcat(resultado, final);

    pthread_mutex_unlock(&pcb->mutex);
    return resultado;
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



int get_apuesta_ronda_pcb(PCB* pcb, int id_core_sorteado, Boolean preflop, Carta* cartas_conocidas,int num_jugadores, int pot, int saldo_apuesta_total, Boolean ronda_igualar) {
    Carta** cartas;
    int num_cartas;
    // Juntar cartas de la mano con las de la mesa
    if (preflop == FALSE) {
        cartas = malloc(sizeof(Carta*) * (pcb -> prioridad + 5));
        num_cartas = pcb -> prioridad + 5;
        for (int i = 0; i < pcb -> prioridad; i++) {
           cartas[i] = &(pcb -> cartas[i]);
        }
        for (int i = 0; i < 5; i++) {
            cartas[pcb -> prioridad + i] = &(cartas_conocidas[i]);
        }
    }else {
       cartas = malloc(sizeof(Carta*) * pcb -> prioridad);
       num_cartas = pcb -> prioridad;
       for (int i = 0; i < num_cartas; i++) {
           cartas[i] = &(pcb -> cartas[i]);
       }
    }

    int multiplicador_afinidad_core = 1;
    if (id_core_sorteado == pcb -> indice_ultimo_core_visitado) {
        multiplicador_afinidad_core = 2;
    }
    // Hacer un farol con la probabilidad de la agresividad
    if(evento_con_probabilidad(pcb -> agresividad * (float)multiplicador_afinidad_core)) {
        free(cartas);
        print_all_in();
        return pcb -> saldo;
    }

    int dinero_necesario_apostar = saldo_apuesta_total - pcb->apuesta_total_partida;
    int dinero_a_apostar = get_dinero_a_apostar(cartas, num_cartas, pot, dinero_necesario_apostar, num_jugadores, preflop, ronda_igualar);
    // No apostar más de lo que se tiene
    if(dinero_a_apostar >= pcb -> saldo) {
        dinero_a_apostar = pcb -> saldo;
    }
    free(cartas);
    return dinero_a_apostar;
}

// copiar los punteros de los PCBs
PCBArray* shallow_copy_pcb_array(PCBArray* original) {
    if (original == NULL) return NULL;

    PCBArray* copia = (PCBArray*)malloc(sizeof(PCBArray));
    copia->cantidad = original->cantidad;

    // Reservar memoria solo para el array de punteros
    copia->pcbs = (PCB**)malloc(sizeof(PCB*) * original->cantidad);

    // Copiar los punteros directamente
    for (int i = 0; i < original->cantidad; i++) {
        copia->pcbs[i] = original->pcbs[i];
    }

    return copia;
}

void set_ultimo_core_visitado_pcb(PCB* pcb, int id_core) {
    pthread_mutex_lock(&pcb -> mutex);
    pcb -> indice_ultimo_core_visitado = id_core;
    pthread_mutex_unlock(&pcb -> mutex);
}

