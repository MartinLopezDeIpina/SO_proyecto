//
// Created by martin on 10/29/24.
//

#ifndef PCB_H
#define PCB_H
#include "Boolean.h"
#include "Poker/Carta.h"
#include "utils.h"
#include <pthread.h>

typedef enum {
    LISTO,
    EJECUTANDO,
    ESPERANDO,
    TERMINADO,
    SALDO_INSUFICIENTE
} EstadoProceso;

typedef struct {
    int pid;
    int prioridad;
    int num_instruccion_actual;
    int num_instrucciones;

    EstadoProceso estado;
    pthread_mutex_t mutex;

    int saldo;
    int saldo_ejecucion;
    int min_saldo_entrar_core;
    float agresividad;
    Carta* cartas;
    int apuesta_total_partida;

} PCB;

typedef struct {
    PCB** pcbs;
    int cantidad;
} PCBArray;

void init_pcb(PCB* pcb, int pid, int prioridad);
void ejecutar_instruccion_proceso(PCB* pcb);
void printear_instrucciones_ejecutadas(PCB* pcb);

Boolean proceso_esta_en_estado(PCB* pcb, EstadoProceso estado);
Boolean proceso_esta_listo(PCB* pcb);
Boolean proceso_saldo_ejecucion_insuficiente(PCB* pcb);
Boolean proceso_ha_terminado(PCB* pcb);
Boolean proceso_saldo_suficiente_para_entrar_core(PCB* pcb);

void set_estado_proceso(PCB* pcb, EstadoProceso estado);
void set_estado_proceso_listo(PCB* pcb);
void set_estado_proceso_ejecutando(PCB* pcb);

void incrementar_saldo_ejecucion(PCB* pcb, int saldo);
void decrementar_saldo_ejecucion(PCB* pcb, int saldo);
void incrementar_saldo(PCB* pcb, int saldo);
void decrementar_saldo(PCB* pcb, int saldo);
void asignar_saldo_ejecucion(PCB* pcb);
int get_apuesta_ronda_pcb(PCB* pcb, Boolean preflop, Carta* cartas_conocidas,int num_jugadores, int pot, int saldo_apuesta_actual);



#endif //PCB_H
