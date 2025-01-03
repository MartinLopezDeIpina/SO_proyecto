//
// Created by martin on 12/30/24.
//

#include "MMU.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "PhysicalMemory.h"
#include "../DebugPrints.h"
#include "../globals.h"
uint64_t get_current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

void init_entrada_TLB(EntradaTLB* entrada, int pid_proceso, uint32_t dir_pag_lógica, uint32_t dir_pag_física) {
    entrada->pid_proceso = pid_proceso;
    //dir_pag_lógica va de direcciones en uno a uno, no de palabras en 4 bytes.
    entrada->dir_pag_lógica = dir_pag_lógica;
    entrada->dir_pag_física = dir_pag_física;
    entrada->timestamp_ultimo_acceso = get_current_time();
}

void init_mmu(MMU* mmu, PhysicalMemory* pm) {
    // En caso de que no esté la página, -1 se considera como un fallo de página.
    for (int i = 0; i < SIZE_TLB; i++) {
        EntradaTLB* entrada = (EntradaTLB*)malloc(sizeof(EntradaTLB));
        entrada -> pid_proceso = -1;
        entrada -> dir_pag_lógica = -1;
        entrada -> dir_pag_física = -1;
        entrada->timestamp_ultimo_acceso = 0;
        mmu->TLB[i] = entrada;
    }
    mmu->pm = pm;
    pthread_mutex_init(&mmu->mutex, NULL);
}

uint32_t get_dir_marco(MMU* mmu, int pid_proceso, uint32_t dir_pag_lógica) {
    pthread_mutex_lock(&mmu->mutex);

    for (int i = 0; i < SIZE_TLB; i++) {
        if (mmu->TLB[i]->pid_proceso == pid_proceso && mmu->TLB[i]->dir_pag_lógica == dir_pag_lógica) {
            mmu->TLB[i]->timestamp_ultimo_acceso = get_current_time();
            uint32_t dir_fisica = mmu->TLB[i]->dir_pag_física;
            pthread_mutex_unlock(&mmu->mutex);
            return dir_fisica;
        }
    }

    pthread_mutex_unlock(&mmu->mutex);
    return -1;
}

void set_variables_TLB(MMU* mmu, int index_TLB, int pid_proceso, uint32_t dir_pag_lógica, uint32_t dir_pag_física) {
    mmu->TLB[index_TLB]->pid_proceso = pid_proceso;
    mmu->TLB[index_TLB]->dir_pag_lógica = dir_pag_lógica;
    mmu->TLB[index_TLB]->dir_pag_física = dir_pag_física;
    mmu->TLB[index_TLB]->timestamp_ultimo_acceso = get_current_time();
}

void set_entrada_TLB(MMU* mmu, int pid_proceso, uint32_t dir_pag_lógica, uint32_t dir_pag_física) {
    pthread_mutex_lock(&mmu->mutex);

    uint64_t min_timestamp = mmu->TLB[0]->timestamp_ultimo_acceso;
    int index_min_timestamp = 0;
    // Si hay una entrada vacía, la reemplaza.
    for (int i = 0; i < SIZE_TLB; i++) {
        if (mmu->TLB[i]->pid_proceso == -1) {
            set_variables_TLB(mmu, i, pid_proceso, dir_pag_lógica, dir_pag_física);
            pthread_mutex_unlock(&mmu->mutex);
            return;
        }
        if (mmu->TLB[i]->timestamp_ultimo_acceso < min_timestamp) {
            min_timestamp = mmu->TLB[i]->timestamp_ultimo_acceso;
            index_min_timestamp = i;
        }
    }
    // Si no hay una entrada vacía, coger el last recently used mirando el timestamp de cuando se accedió la última vez.
    set_variables_TLB(mmu, index_min_timestamp, pid_proceso, dir_pag_lógica, dir_pag_física);

    pthread_mutex_unlock(&mmu->mutex);
}

void liberar_entradas_proceso(MMU* mmu, int pid_proceso) {
    pthread_mutex_lock(&mmu->mutex);

    for (int i = 0; i < SIZE_TLB; i++) {
        if (mmu->TLB[i]->pid_proceso == pid_proceso) {
            mmu->TLB[i]->pid_proceso = -1;
            mmu->TLB[i]->dir_pag_lógica = -1;
            mmu->TLB[i]->dir_pag_física = -1;
        }
    }

    pthread_mutex_unlock(&mmu->mutex);
}

/*
 * Devuelve un puntero a la dirección de memoria física correspondiente a la dirección lógica del proceso indicado.
 * Si la dirección lógica del marco está en la TLB, se calcula directamente la dirección física.
 * Si la dirección lógica del marco no está en la TLB, se consulta la tabla de páginas.
 */
uint32_t get_dir_fisica_para_dir_logica(MMU* mmu, uint32_t dir_logica, uint32_t* PTBR, int pid_proceso, uint32_t* dir_log_text, uint32_t* dir_logica_data) {
    // Si la dirección lógica no está alineada a 4 bytes printear un error.
    if (dir_logica % 4 != 0) {
        print_error_direccion_no_alineada(dir_logica);
    }

    bool es_dir_text = dir_logica < *dir_logica_data;

    int dir_logica_pagina;
    int indice_pag_data;
    if(es_dir_text) {
        dir_logica_pagina = (dir_logica - *dir_log_text) / (TAMANIO_PAGINA);
    }else {
        int num_direcciones_text = *dir_logica_data - *dir_log_text;
        int num_pags_text = (num_direcciones_text + (TAMANIO_PAGINA) - 1) / (TAMANIO_PAGINA);

        int num_instrucciones_data_antes = (dir_logica - *dir_logica_data)/4;
        indice_pag_data = (num_instrucciones_data_antes + (TAMANIO_PAGINA) - 1) / (TAMANIO_PAGINA);
        dir_logica_pagina = num_pags_text + indice_pag_data;
    }

    uint32_t dir_fisica_marco = get_dir_marco(mmu, pid_proceso, dir_logica_pagina);

    bool fallo_pagina = dir_fisica_marco == -1;
    // Acceder a la tabla de páginas para obtener la dirección física de la página.
    if(fallo_pagina) {
        // dir_logica_pagina debe multiplicarse por 4 porque cada entrada ocupa 4 bytes
        uint32_t dir_tabla_paginas_marco = *PTBR + (dir_logica_pagina * 4);
        dir_fisica_marco = get_valor_en_direccion_de_memoria(mmu->pm, dir_tabla_paginas_marco);

        set_entrada_TLB(mmu, pid_proceso, dir_logica_pagina, dir_fisica_marco);
    }

    uint32_t offset_dentro_de_pagina = 0;
    if(es_dir_text) {
        offset_dentro_de_pagina = dir_logica % TAMANIO_PAGINA;
    }else {
        offset_dentro_de_pagina = (dir_logica - *dir_logica_data) % TAMANIO_PAGINA;
    }

    // dir_fisica_marco ya está en bytes, offset_dentro_de_pagina también está en bytes
    uint32_t dir_fisica = dir_fisica_marco + offset_dentro_de_pagina;

    return dir_fisica;
}

int get_afinidad_mmu_tlb_con_proceso(MMU* mmu, int pid) {
    int cuenta = 0;
    for (int i = 0; i < SIZE_TLB; i++) {
        if (mmu->TLB[i]->pid_proceso == pid) {
            cuenta++;
        }
    }
    return cuenta;
}


