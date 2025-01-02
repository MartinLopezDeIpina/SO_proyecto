//
// Created by martin on 10/29/24.
//

#ifndef PCB_H
#define PCB_H
#include "Boolean.h"
#include "Poker/Carta.h"
#include "utils.h"
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#include "Machine/MMU.h"

typedef enum {
    LISTO,
    EJECUTANDO,
    ESPERANDO,
    TERMINADO,
    SALDO_INSUFICIENTE
} EstadoProceso;

typedef struct {
  // Apuntan a la dirección virtual del comienzo de cada segmento -> un entero que representa la dirección virtual
  uint32_t* code;
  uint32_t* data;
  // Apunta a la dirección física de la tabla de páginas -> un entero dentro del array de direcciones físicas que representa la dirección física de la primera página del segmento.
  uint32_t* pgb;
}mm;

typedef struct {
    int registros[16];
    int PC;
    int IR;
}EstadoEjecucionProceso;

void init_estado_ejecucion_proceso(EstadoEjecucionProceso* estado_ejecucion_proceso);

typedef struct {
    int pid;
    int prioridad;
    int num_instruccion_actual;
    int num_instrucciones;
    int indice_ultimo_core_visitado;

    EstadoProceso estado;
    pthread_mutex_t mutex;

    int saldo;
    int saldo_ejecucion;
    int min_saldo_entrar_core;
    float agresividad;
    Carta* cartas;
    int apuesta_total_partida;

    mm* mm_pcb;
    EstadoEjecucionProceso* estado_ejecucion_proceso;

} PCB;


typedef struct {
    PCB** pcbs;
    int cantidad;
} PCBArray;
PCBArray* shallow_copy_pcb_array(PCBArray* original);

void init_pcb(PCB* pcb, int pid, int prioridad);
void avanzar_ejecucion_proceso(PCB* pcb);
void printear_instrucciones_ejecutadas(PCB* pcb);

Boolean proceso_esta_en_estado(PCB* pcb, EstadoProceso estado);
Boolean proceso_esta_listo(PCB* pcb);
Boolean proceso_saldo_ejecucion_insuficiente(PCB* pcb);
Boolean proceso_ha_terminado(PCB* pcb);
Boolean proceso_saldo_suficiente_para_entrar_core(PCB* pcb);

void set_estado_proceso(PCB* pcb, EstadoProceso estado);
void set_estado_proceso_listo(PCB* pcb);
void set_estado_proceso_ejecutando(PCB* pcb);
void set_estado_proceso_terminado(PCB* pcb);
void set_ultimo_core_visitado_pcb(PCB* pcb, int id_core);

void incrementar_saldo_ejecucion(PCB* pcb, int saldo);
void decrementar_saldo_ejecucion(PCB* pcb, int saldo);
void incrementar_saldo(PCB* pcb, int saldo);
void decrementar_saldo(PCB* pcb, int saldo);
void asignar_saldo_ejecucion(PCB* pcb);
int get_apuesta_ronda_pcb(PCB* pcb, int id_core_sorteado, Boolean preflop, Carta* cartas_conocidas,int num_jugadores, int pot, int saldo_apuesta_actual, Boolean ronda_igualar);



#endif //PCB_H
