//
// Created by martin on 12/30/24.
//

#ifndef HILOHARDWARE_H
#define HILOHARDWARE_H
#include <stdint.h>

#include "../PCB.h"
#include <bits/pthreadtypes.h>

#include "MMU.h"

typedef struct {
    // Mentras que el id del core es absoluto, el id del hilo es relativo al core en el que se encuentra.
    int id_hilo;

    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    pthread_mutex_t mutex_acceso_hilo;

    PCB* current_process;

    uint32_t* PTBR;
    int PC;
    int IR;

    MMU* mmu;

    int registros[16];
    int pid_registros[16];
}HiloHardware;

void init_hilo_hardware(HiloHardware* hilo_hardware, int id_hilo);
void ejecutar_instruccion(HiloHardware* hilo);
Boolean hilo_esta_ocioso(HiloHardware* hilo);
Boolean hilo_esta_vacio(HiloHardware* hilo);
void printear_instrucciones_ejecutadas_hilo(HiloHardware* hilo);
void notificar_tick_clock_hilo(HiloHardware* hilo);
void vaciar_hilo(HiloHardware* hilo);
void vaciar_hilo_y_set_estado(HiloHardware* hilo, EstadoProceso estado);
void asignar_proceso_a_hilo(HiloHardware* hilo, PCB* pcb);
Boolean proceso_hilo_saldo_ejecucion_insuficiente(HiloHardware* hilo);


#endif //HILOHARDWARE_H
