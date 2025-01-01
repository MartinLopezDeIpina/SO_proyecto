//
// Created by martin on 12/30/24.
//

#include "PhysicalMemory.h"

#include <stdio.h>

#include "../globals.h"

#include <stdlib.h>

void init_physical_memory(PhysicalMemory* pm) {
    pm->memoria = (uint32_t*)malloc(NUM_DIRECCIONES * sizeof(uint32_t));
    for (int i = 0; i < NUM_DIRECCIONES; i++) {
        pm->memoria[i] = 0;
    }
}

/**
 *Va iterando en la memoria física de página en página, mirando si la dirección de la página está en la tabla de páginas.
 */
uint32_t get_dir_pag_libre(PhysicalMemory* pm) {
    // Quitar el espacio de las páginas de la tabla de páginas. -> num páginas * 4 es lo que ocupa la tabla de páginas.
    int num_direcciones_paginas = (NUM_DIRECCIONES - NUM_PAGINAS)/TAMANIO_PAGINA;
    for (int i = 0; i < num_direcciones_paginas; i++) {
        uint32_t dir_pag = DIR_PRIMERA_PAGINA + i * TAMANIO_PAGINA;
        // buscar si la tabla de páginas tiene la dirección de la página.
        for(int j = 0; j < NUM_PAGINAS; j++) {
            if (pm->memoria[j] == dir_pag) {
                break;
            }
            // si llega al final de la tabla entonces ninguna página tiene la dirección.
            if (j == NUM_PAGINAS - 1) {
                return dir_pag;
            }
        }
    }
    return NULL;
}

void escribir_direccion_en_tabla_paginas(PhysicalMemory* pm, uint32_t dir_pag_tabla_paginas, uint32_t dir_pag) {
    pm -> memoria[dir_pag_tabla_paginas] = (uint32_t)dir_pag;

    printf("En la dirección %d se ha escrito el valor %d\n", dir_pag_tabla_paginas, pm->memoria[dir_pag_tabla_paginas]);
}

void escribir_valor_en_direccion(PhysicalMemory* pm, uint32_t dir, uint32_t valor) {
    pm -> memoria[dir] = valor;

    printf("En la dirección %d se ha escrito el valor %X\n", dir, pm->memoria[dir]);
}


//Devuelve la dirección de memoria donde se encuentra la entrada a la primera página del nuevo proceso.
uint32_t get_entrada_tabla_paginas_para_nuevo_proceso(PhysicalMemory* pm, int num_paginas_proceso) {
    Boolean encontrado = FALSE;
    // Se busca la primera entrada libre en la tabla de páginas.
    uint32_t dir_primera_pagina = (uint32_t)malloc(sizeof(uint32_t));
    for(int i = 0; i < NUM_PAGINAS; i++) {
       //comprobar si num_paginas_proceso están libres
       dir_primera_pagina = i;
       for(int j = 0; j < num_paginas_proceso; j++) {
          uint32_t* dir_pag = &pm->memoria[i+j];
          if (*dir_pag != (uint32_t)0) {
            break;
          }
          if (j == num_paginas_proceso - 1) {
            encontrado = TRUE;
          }
       }
       if(encontrado == TRUE) {
           break;
       }
    }
    if (encontrado == FALSE) {
        return NULL;
    }
    for(int j = 0; j < num_paginas_proceso; j++) {
       uint32_t dir_pag_table = dir_primera_pagina + j;
       uint32_t dir_pag_libre = get_dir_pag_libre(pm);
       escribir_direccion_en_tabla_paginas(pm, dir_pag_table, dir_pag_libre);
    }
    return dir_primera_pagina;
}

