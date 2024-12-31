//
// Created by martin on 12/30/24.
//

#include "MMU.h"

#include <stdlib.h>

#include "../globals.h"

void init_mmu(MMU* mmu) {
    mmu->TLB = (uint32_t*)malloc(SIZE_TLB * sizeof(uint32_t));
    // En caso de que no esté la página, -1 se considera como un fallo de página.
    for (int i = 0; i < SIZE_TLB; i++) {
        mmu->TLB[i] = (uint32_t)-1;
    }
}

uint32_t get_entrada_TLB(MMU* mmu, uint32_t entrada) {
    return mmu->TLB[entrada];
}

void set_entrada_TLB(MMU* mmu, uint32_t entrada, uint32_t valor) {
    mmu->TLB[entrada] = valor;
}
