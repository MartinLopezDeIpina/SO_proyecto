//
// Created by martin on 12/23/24.
//

#include <stdio.h>
#include <pthread.h>
#include "Core.h"

#include <stdlib.h>
#include <string.h>

/*
 * El objeto Core puede ser accedido por más de un hilo, por ejemplo el scheduler para asignar y quitar procesos, y el clock para notificar el tick.
 * El mutex mutex_acceso_core es para proteger el acceso al core.
 * El mutex mutex es para sincronizar la ejecución de instrucciones con los ticks del reloj.
 */

char* print_estado_core(Core* core) {
    char* resultado = malloc(1000);
    pthread_mutex_lock(&core->mutex_acceso_core);

    sprintf(resultado, "Core %d:\n", core->id_core);
    for(int i = 0; i < core->num_threads_core; i++) {
        char* estado_hilo = printear_instrucciones_ejecutadas_hilo(&core->hilos[i]);
        strcat(resultado, estado_hilo);
        free(estado_hilo);
    }
    strcat(resultado, "\n");

    pthread_mutex_unlock(&core->mutex_acceso_core);
    return resultado;
}

void init_hilos_hardware(int num_threads_core, HiloHardware* hilos, PhysicalMemory* pm) {
    for (int i = 0; i < num_threads_core; i++) {
        init_hilo_hardware(&hilos[i], i, pm);
    }
}

void init_core(int id_core, Core* core, PhysicalMemory* pm, int num_threads_core) {
    core -> id_core = id_core;

    core -> num_threads_core = num_threads_core;
    core -> hilos = (HiloHardware*)malloc(num_threads_core * sizeof(HiloHardware));
    init_hilos_hardware(num_threads_core, core -> hilos, pm);

    pthread_mutex_init(&core->mutex_acceso_core, NULL);
}

char* notificar_tick_clock_core(Core* core) {
    char* resultado = malloc(2000);
    resultado[0] = '\0';

    char* estado = print_estado_core(core);
    strcpy(resultado, estado);
    free(estado);

    for(int i = 0; i < core -> num_threads_core; i++) {
        notificar_tick_clock_hilo(&core -> hilos[i]);
    }

    return resultado;
}

// el core está ocioso si al menos uno de sus hilos está ocioso -> se puede añadir un proceso más la core
Boolean core_esta_ocioso(Core* core) {
    Boolean ocioso = FALSE;

    for(int i = 0; i < core -> num_threads_core; i++) {
        if(hilo_esta_ocioso(&core -> hilos[i])) {
            ocioso = TRUE;
            break;
        }
    }

    return ocioso;
}

int get_num_hilos_ociosos_core(Core* core) {
    int cuenta = 0;
    for(int i = 0; i < core -> num_threads_core; i++) {
        if(hilo_esta_ocioso(&core -> hilos[i])) {
            cuenta++;
        }
    }
    return cuenta;
}

void vaciar_hilos_sin_saldo_suficiente(Core* core) {
    for(int i = 0; i < core -> num_threads_core; i++) {
        if (hilo_esta_vacio(&core->hilos[i]) == FALSE && proceso_hilo_saldo_ejecucion_insuficiente(&core->hilos[i]) == TRUE) {
            vaciar_hilo_y_set_estado(&core->hilos[i], LISTO);
        }
    }
}

// asignar el proceso al primer hilo del core disponible
void asignar_proceso_a_core(Core* core, PCB* pcb) {
    pthread_mutex_lock(&core->mutex_acceso_core);

    int hilo_mayor_afinidad = -1;
    int num_entradas_tlb_proceso = -1;
    for(int i = 0; i < core->num_threads_core; i ++) {
        if(hilo_esta_ocioso(&core -> hilos[i])) {
            int afinidad_con_hilo = get_afinidad_hilo_con_proceso(&core->hilos[i], pcb->pid);
            if(afinidad_con_hilo > num_entradas_tlb_proceso) {
                hilo_mayor_afinidad = i;
            }
        }
    }

    asignar_proceso_a_hilo(&core -> hilos[hilo_mayor_afinidad], pcb);
    set_ultimo_core_visitado_pcb(pcb, core -> id_core);

    pthread_mutex_unlock(&core->mutex_acceso_core);
}

int vaciar_hilos_terminados_core(Core* core, int* pid_procesos_terminados, int index_actual) {
    int cuenta = 0;
    for(int i = 0; i < core -> num_threads_core; i++) {
        if(hilo_esta_vacio(&core -> hilos[i]) == FALSE && proceso_ha_terminado(core -> hilos[i].current_process) == TRUE) {
            pid_procesos_terminados[index_actual + cuenta] = core -> hilos[i].current_process -> pid;
            vaciar_hilo(&core -> hilos[i]);
            cuenta++;
        }
    }
    return cuenta;
}

