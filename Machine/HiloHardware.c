//
// Created by martin on 12/30/24.
//

#include "HiloHardware.h"

#include <pthread.h>
#include <stdio.h>

#include "../Boolean.h"
#include "PhysicalMemory.h"
#include "../DebugPrints.h"

// Sacar la dirección física usando la MMU en el hilo hardware.
uint32_t get_instruccion_proceso(HiloHardware* hilo) {
    uint32_t dir_fisica = get_dir_fisica_para_dir_logica(
        hilo->mmu,
        hilo->PC,
        hilo->PTBR,
        hilo->current_process->pid,
        hilo->current_process->mm_pcb->code,
        hilo->current_process->mm_pcb->data
        );
    uint32_t instruccion = get_valor_en_direccion_de_memoria(hilo->mmu->pm, dir_fisica);
    return instruccion;
}

uint32_t get_registro(HiloHardware* hilo, int num_registro) {
    return hilo->registros[num_registro];
}
uint32_t set_registro(HiloHardware* hilo, int num_registro, uint32_t valor) {
    hilo->registros[num_registro] = valor;
}

void ejecutar_funcion_ld(HiloHardware* hilo, uint32_t instruccion) {
    // Obtener el segundo dígito
    int num_registro = (instruccion & 0x0F000000) >> 24;
    uint32_t dir_memoria = instruccion & 0x00FFFFFF;

    uint32_t dir_fisica = get_dir_fisica_para_dir_logica(
        hilo->mmu,
        dir_memoria,
        hilo->PTBR,
        hilo->current_process->pid,
        hilo->current_process->mm_pcb->code,
        hilo->current_process->mm_pcb->data
        );

    uint32_t valor = get_valor_en_direccion_de_memoria(hilo->mmu->pm, dir_fisica);

    set_registro(hilo, num_registro, valor);

    print_instruccion_ld(hilo->current_process->pid, num_registro, valor);
}

void ejecutar_funcion_add(HiloHardware* hilo, uint32_t instruccion) {
    uint32_t registro_destino = (instruccion & 0x0F000000) >> 24;
    uint32_t registro_fuente1 = (instruccion & 0x00F00000) >> 20;
    uint32_t registro_fuente2 = (instruccion & 0x000F0000) >> 16;

    uint32_t suma = get_registro(hilo, registro_fuente1) + get_registro(hilo, registro_fuente2);

    hilo->registros[registro_destino] = suma;

    print_instruccion_add(hilo->current_process->pid, get_registro(hilo, registro_fuente1), get_registro(hilo, registro_fuente2), suma);
}

void ejecutar_funcion_st(HiloHardware* hilo, uint32_t instruccion) {
    uint32_t num_registro = (instruccion & 0x0F000000) >> 24;
    uint32_t dir_memoria = instruccion & 0x00FFFFFF;

    uint32_t dir_fisica = get_dir_fisica_para_dir_logica(
        hilo->mmu,
        dir_memoria,
        hilo->PTBR,
        hilo->current_process->pid,
        hilo->current_process->mm_pcb->code,
        hilo->current_process->mm_pcb->data
        );

    uint32_t valor = get_registro(hilo, num_registro);

    // usar la función para lockear el mutex en lugar de acceder directamente al memoria.
    escribir_valor_en_direccion(hilo->mmu->pm, dir_fisica, valor);

    print_instruccion_st(hilo->current_process->pid, dir_fisica, valor);
}

void limpiar_registros_proceso(HiloHardware* hilo, int pid) {
    for(int i = 0; i < 16; i++) {
        hilo->registros[i] = -1;
    }
    hilo -> PC = -1;
    hilo -> IR = -1;

    liberar_entradas_proceso(hilo->mmu, pid);
}

void liberar_todas_las_paginas_proceso(HiloHardware* hilo) {
    // Las direcciones ya vienen en bytes
    int num_bytes_text = *hilo->current_process->mm_pcb->data - *hilo->current_process->mm_pcb->code;
    // El tamanio de las páginas está en bytes
    int num_pags_text = (num_bytes_text + TAMANIO_PAGINA - 1) / TAMANIO_PAGINA;

    int num_bytes_data = hilo->current_process->num_instrucciones_data * 4;
    int num_pags_data = (num_bytes_data + TAMANIO_PAGINA - 1) / TAMANIO_PAGINA;

    int num_pags_totales = num_pags_text + num_pags_data;

    liberar_memoria_paginas(hilo->mmu->pm, *hilo->PTBR, num_pags_totales);
}

void ejecutar_funcion_exit(HiloHardware* hilo, uint32_t instruccion) {

    liberar_todas_las_paginas_proceso(hilo);

    limpiar_registros_proceso(hilo, hilo->current_process->pid);

    // El scheduler se encarga de eliminar el proceso de la cola de procesos.
    set_estado_proceso_terminado(hilo->current_process);

    print_instruccion_exit(hilo->current_process->pid);
}

void ejecutar_funcion_instruccion(HiloHardware* hilo, uint32_t instruccion) {
    hilo->IR = instruccion;

    // obtener el primer dígito con una máscara y desplazando 28 bits -> cada dígito hexadecimal son 4 bits
    uint32_t primer_digito = (instruccion & 0xF0000000) >> 28;

    switch(primer_digito) {
        case(0):
            ejecutar_funcion_ld(hilo, instruccion);
            break;
        case(1):
            ejecutar_funcion_st(hilo, instruccion);
            break;
        case(2):
            ejecutar_funcion_add(hilo, instruccion);
            break;
        case(0xF):
            ejecutar_funcion_exit(hilo, instruccion);
            break;
        default:
            printf("error: instrucción %X no válida\n", primer_digito);
            break;
    }
}

void ejecutar_instruccion(HiloHardware* hilo) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    uint32_t instruccion = get_instruccion_proceso(hilo);
    print_ejecutando_instruccion(hilo->current_process->pid, instruccion);
    ejecutar_funcion_instruccion(hilo, instruccion);
    hilo->PC += 4;
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
    hilo_hardware->mmu = mmu;

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

void copiar_registros_hilo_hardware_en_objeto_PCB(HiloHardware* hilo) {
    hilo->current_process->estado_ejecucion_proceso->PC = hilo->PC;
    hilo->current_process->estado_ejecucion_proceso->IR = hilo->IR;
    for(int i = 0; i < 16; i++) {
        hilo->current_process->estado_ejecucion_proceso->registros[i] = hilo->registros[i];
    }
}

void vaciar_hilo_y_set_estado(HiloHardware* hilo, EstadoProceso estado) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    copiar_registros_hilo_hardware_en_objeto_PCB(hilo);
    set_estado_proceso(hilo -> current_process, estado);
    hilo -> current_process = NULL;
    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);
}

/*
 * Cargar los registros del objeto PCB en el hilo, en una simulación más realista estarían en la memoria física.
 */
void cargar_registros_proceso_en_hilo(HiloHardware* hilo, PCB* pcb) {
    hilo -> PC = pcb->estado_ejecucion_proceso->PC;
    hilo -> IR = pcb->estado_ejecucion_proceso->IR;
    hilo -> PTBR = pcb->mm_pcb->pgb;
    for(int i = 0; i < 16; i++) {
        hilo -> registros[i] = pcb->estado_ejecucion_proceso->registros[i];
        hilo -> pid_registros[i] = pcb->pid;
    }
}

void asignar_proceso_a_hilo(HiloHardware* hilo, PCB* pcb) {
    pthread_mutex_lock(&hilo->mutex_acceso_hilo);
    hilo -> current_process = pcb;
    hilo -> PTBR = pcb -> mm_pcb -> pgb;

    set_estado_proceso_ejecutando(pcb);
    pthread_mutex_unlock(&hilo->mutex_acceso_hilo);
}

int get_afinidad_hilo_con_proceso(HiloHardware* hilo, int pid) {
    return get_afinidad_mmu_tlb_con_proceso(hilo->mmu, pid);
}
