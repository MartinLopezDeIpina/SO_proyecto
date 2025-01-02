//
// Created by martin on 12/30/24.
//

#include "HiloHardware.h"

#include <pthread.h>
#include <stdio.h>

#include "../Boolean.h"

// Primero busca la dirección de la página en la tabla de páginas, si no la encuentra la busca en la memoria física y la asigna a la tabla de páginas.
uint32_t get_instruccion_proceso(HiloHardware* hilo) {
    uint32_t* dir_fisica = get_dir_fisica_para_dir_logica(hilo->mmu, hilo->PC,hilo->PTBR, hilo->current_process->pid, hilo->current_process->mm_pcb->code, hilo->current_process->mm_pcb->data);
    return *dir_fisica;
}

void ejecutar_funcion_instruccion(uint32_t instruccion) {
   //todo
}

void ejecutar_instruccion(HiloHardware* hilo) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    uint32_t instruccion = get_instruccion_proceso(hilo);
    ejecutar_funcion_instruccion(instruccion);
    avanzar_ejecucion_proceso(hilo -> current_process);
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

void init_hilo_hardware(HiloHardware* hilo_hardware, int id_hilo, PhysicalMemory* pm) {
    hilo_hardware -> id_hilo = id_hilo;
    hilo_hardware -> current_process = NULL;
    for(int i = 0; i < 16; i++) {
        hilo_hardware -> registros[i] = 0;
        hilo_hardware -> pid_registros[i] = -1;
    }
    MMU* mmu = malloc(sizeof(MMU));
    init_mmu(mmu, pm);

    pthread_mutex_init(&hilo_hardware->mutex_acceso_hilo, NULL);
    pthread_mutex_init(&hilo_hardware->mutex, NULL);
    pthread_cond_init(&hilo_hardware->condition, NULL);
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

    //Cargar los registros del proceso en el hilo, en una simulación más realista estarían en la memoria física.
    hilo -> PC = pcb->estado_ejecucion_proceso->PC;
    hilo -> IR = pcb->estado_ejecucion_proceso->IR;
    hilo -> PTBR = pcb->mm_pcb->pgb;
    for(int i = 0; i < 16; i++) {
        hilo -> registros[i] = pcb->estado_ejecucion_proceso->registros[i];
        hilo -> pid_registros[i] = pcb->pid;
    }

    set_estado_proceso_ejecutando(pcb);
    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);
}
