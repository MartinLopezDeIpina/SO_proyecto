//
// Created by martin on 12/30/24.
//

#ifndef MMU_H
#define MMU_H
#include <stdint.h>

typedef struct {
    uint32_t* TLB;
}MMU;

void init_mmu(MMU* mmu);

#endif //MMU_H
