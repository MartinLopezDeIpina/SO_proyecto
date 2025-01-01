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
uint32_t get_entrada_tabla_paginas_para_nuevo_proceso(PhysicalMemory* pm, int num_paginas_proceso);
uint32_t get_dir_pag_libre(PhysicalMemory* pm);
void escribir_direccion_en_tabla_paginas(PhysicalMemory* pm, uint32_t dir_pag_tabla_paginas, uint32_t dir_pag);
void escribir_valor_en_direccion(PhysicalMemory* pm, uint32_t dir, uint32_t valor);

#endif //PHYSICALMEMORY_H
