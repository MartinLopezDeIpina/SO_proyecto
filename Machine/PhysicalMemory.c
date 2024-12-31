//
// Created by martin on 12/30/24.
//

#include "PhysicalMemory.h"
#include "../globals.h"

#include <stdlib.h>

void init_physical_memory(PhysicalMemory* pm) {
    pm->memoria = (uint32_t*)malloc(NUM_DIRECCIONES * sizeof(uint32_t));
    for (int i = 0; i < NUM_DIRECCIONES; i++) {
        pm->memoria[i] = 0;
    }
}
