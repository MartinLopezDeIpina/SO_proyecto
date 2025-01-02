//
// Created by martin on 12/30/24.
//

#ifndef PHYSICALMEMORY_H
#define PHYSICALMEMORY_H
#include <stdint.h>
#include <bits/pthreadtypes.h>

typedef struct {
    uint32_t* memoria;
    // Los procesos podrán acceder a la memoria física de forma concurrente.
    pthread_mutex_t mutex;
}PhysicalMemory;

void init_physical_memory(PhysicalMemory* pm);
uint32_t get_entrada_tabla_paginas_para_nuevo_proceso(PhysicalMemory* pm, int num_paginas_proceso);
uint32_t get_dir_pag_libre_pm(PhysicalMemory* pm);
void escribir_direccion_en_tabla_paginas(PhysicalMemory* pm, uint32_t dir_pag_tabla_paginas, uint32_t dir_pag);
void escribir_valor_en_direccion(PhysicalMemory* pm, uint32_t dir, uint32_t valor);
uint32_t get_valor_en_puntero_a_direccion(PhysicalMemory* pm, uint32_t* puntero_a_direccion);
uint32_t* get_puntero_a_direccion_memoria(PhysicalMemory* pm, uint32_t dir);
void escribir_valor_en_puntero_a_direccion(PhysicalMemory* pm, uint32_t* puntero_a_direccion, uint32_t valor);
void liberar_memoria_paginas(PhysicalMemory* pm, uint32_t dir_fisica_marco, int num_pags);

#endif //PHYSICALMEMORY_H
