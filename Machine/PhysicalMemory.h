//
// Created by martin on 12/30/24.
//

#ifndef PHYSICALMEMORY_H
#define PHYSICALMEMORY_H
#include <stdint.h>

typedef struct {
    uint32_t* memoria;
}PhysicalMemory;

void init_physical_memory(PhysicalMemory* pm);

#endif //PHYSICALMEMORY_H
