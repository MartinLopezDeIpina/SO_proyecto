//
// Created by martin on 12/30/24.
//

#include "HiloHardware.h"

#include <pthread.h>
#include <stdio.h>

#include "../Boolean.h"

void ejecutar_instruccion(HiloHardware* hilo) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    ejecutar_instruccion_proceso(hilo -> current_process);
    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);
}

Boolean hilo_esta_vacio(HiloHardware* hilo) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);

    int hilo_esta_vacio = hilo -> current_process == NULL;

    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);

    if(hilo_esta_vacio) {
        return TRUE;
    }else {
        return FALSE;
    }
}

Boolean hilo_esta_ocioso(HiloHardware* hilo) {
    int hilo_esta_ocioso = hilo_esta_vacio(hilo) || proceso_ha_terminado(hilo -> current_process);

    if(hilo_esta_ocioso) {
        return TRUE;
    }else {
        return FALSE;
    }
}

void notificar_tick_clock_hilo(HiloHardware* hilo) {
    pthread_mutex_lock(&hilo->mutex);
    pthread_cond_signal(&hilo->condition);
    pthread_mutex_unlock(&hilo->mutex);
}

void* funcion_hilo_hardware(void* arg) {
    HiloHardware* hilo = (HiloHardware*)arg;

    while(1) {
        pthread_mutex_lock(&hilo->mutex);
        pthread_cond_wait(&hilo->condition, &hilo->mutex);

        Boolean hilo_ocioso = hilo_esta_ocioso(hilo);
        if (hilo_ocioso == FALSE) {
            ejecutar_instruccion(hilo);
        }

        pthread_mutex_unlock(&hilo->mutex);
    }
    return NULL;
}

void init_hilo_hardware(HiloHardware* hilo_hardware, int id_hilo) {
    hilo_hardware -> id_hilo = id_hilo;
    pthread_mutex_init(&hilo_hardware->mutex_acceso_hilo, NULL);
    pthread_create(&hilo_hardware->thread, NULL, funcion_hilo_hardware, (void*)hilo_hardware);
}

void printear_instrucciones_ejecutadas_hilo(HiloHardware* hilo) {
    printf("Hilo %d: ", hilo->id_hilo);
   if (hilo_esta_ocioso(hilo)) {
       printf("Ocioso\n");
   }else {
       printear_instrucciones_ejecutadas(hilo->current_process);
   }
}

Boolean proceso_hilo_saldo_ejecucion_insuficiente(HiloHardware* hilo) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    Boolean saldo_insuficiente = proceso_saldo_ejecucion_insuficiente(hilo -> current_process);
    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);

    return saldo_insuficiente;
}

void vaciar_hilo(HiloHardware* hilo) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    hilo -> current_process = NULL;
    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);
}

void vaciar_hilo_y_set_estado(HiloHardware* hilo, EstadoProceso estado) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    set_estado_proceso(hilo -> current_process, estado);
    hilo -> current_process = NULL;
    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);
}

void asignar_proceso_a_hilo(HiloHardware* hilo, PCB* pcb) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    hilo -> current_process = pcb;
    set_estado_proceso_ejecutando(pcb);
    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);
}
