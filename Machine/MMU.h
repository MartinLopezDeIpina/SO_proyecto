//
// Created by martin on 12/30/24.
//

#ifndef MMU_H
#define MMU_H
#include <stdint.h>
#include <bits/pthreadtypes.h>

#include "PhysicalMemory.h"
#include "../globals.h"

typedef struct {
    int pid_proceso;
    uint32_t dir_pag_lógica;
    uint32_t dir_pag_física;
    uint64_t timestamp_ultimo_acceso;
}EntradaTLB;

typedef struct {
    EntradaTLB* TLB[SIZE_TLB];
    PhysicalMemory* pm;
    // La MMU puede ser accedida por varios hilos a la vez.
    pthread_mutex_t mutex;
}MMU;

void init_mmu(MMU* mmu, PhysicalMemory* pm);
uint32_t* get_dir_fisica_para_dir_logica(MMU* mmu, uint32_t dir_logica, uint32_t* PTBR, int pid_proceso, uint32_t* dir_log_text, uint32_t* dir_logica_data);

#endif //MMU_H
