//
// Created by martin on 12/30/24.
//

#include "PhysicalMemory.h"

#include <stdio.h>

#include "../globals.h"

#include <stdlib.h>
#include <pthread.h>

void init_physical_memory(PhysicalMemory* pm) {
    pm->memoria = (uint32_t*)malloc(NUM_DIRECCIONES * sizeof(uint32_t));
    for (int i = 0; i < NUM_DIRECCIONES; i++) {
        pm->memoria[i] = 0;
    }
    pthread_mutex_init(&pm->mutex, NULL);
}

/**
 *Va iterando en la memoria física de página en página, mirando si la dirección de la página está en la tabla de páginas.
 *Función interna sin mutex, llamarla solo si se tiene el mutex.
 */
uint32_t get_dir_pag_libre(PhysicalMemory* pm) {
    for (int i = 0; i < NUM_DIRECCIONES_PAGINAS; i++) {
        uint32_t dir_pag = DIR_PRIMERA_PAGINA + i * TAMANIO_PAGINA;
        // buscar si la tabla de páginas tiene la dirección de la página.
        for(int j = 0; j < NUM_PAGINAS; j++) {
            if (pm->memoria[j] == dir_pag) {
                break;
            }
            // si llega al final de la tabla entonces ninguna página tiene la dirección.
            if (j == NUM_PAGINAS - 1) {
                pthread_mutex_unlock(&pm->mutex);
                return dir_pag;
            }
        }
    }
    return NULL;
}

uint32_t get_dir_pag_libre_pm(PhysicalMemory* pm) {
    pthread_mutex_lock(&pm->mutex);

    uint32_t dir_pag = get_dir_pag_libre(pm);

    pthread_mutex_unlock(&pm->mutex);
    return dir_pag;
}

void escribir_direccion_en_tabla_paginas(PhysicalMemory* pm, uint32_t dir_pag_tabla_paginas, uint32_t dir_pag) {
    pthread_mutex_lock(&pm->mutex);

    pm -> memoria[dir_pag_tabla_paginas] = (uint32_t)dir_pag;
    printf("** En la dirección %d se ha escrito el valor %d **\n", dir_pag_tabla_paginas, pm->memoria[dir_pag_tabla_paginas]);

   pthread_mutex_unlock(&pm->mutex);
}


void escribir_valor_en_puntero_a_direccion(PhysicalMemory* pm, uint32_t* puntero_a_direccion, uint32_t valor) {
    pthread_mutex_lock(&pm->mutex);

    *puntero_a_direccion = valor;

    pthread_mutex_unlock(&pm->mutex);
}

void escribir_valor_en_direccion(PhysicalMemory* pm, uint32_t dir, uint32_t valor) {
    pthread_mutex_lock(&pm->mutex);

    pm -> memoria[dir] = valor;
    printf("En la dirección %d se ha escrito el valor %X\n", dir, pm->memoria[dir]);

    pthread_mutex_unlock(&pm->mutex);
}

/*
 * Dada una dirección de memoria, devuelve un puntero al array de memoria usando la dirección comom índice.
 */
uint32_t* get_puntero_a_direccion_memoria(PhysicalMemory* pm, uint32_t dir) {
    pthread_mutex_lock(&pm->mutex);

    uint32_t* puntero = &(pm->memoria[dir]);

    pthread_mutex_unlock(&pm->mutex);
    return puntero;
}

// usar la función para lockear el mutex en lugar de acceder directamente al puntero.
uint32_t get_valor_en_puntero_a_direccion(PhysicalMemory* pm, uint32_t* puntero_a_direccion) {
    pthread_mutex_lock(&pm->mutex);

    uint32_t valor = *puntero_a_direccion;

    pthread_mutex_unlock(&pm->mutex);
    return valor;
}


//Devuelve la dirección de memoria donde se encuentra la entrada a la primera página del nuevo proceso.
uint32_t get_entrada_tabla_paginas_para_nuevo_proceso(PhysicalMemory* pm, int num_paginas_proceso) {
    pthread_mutex_lock(&pm->mutex);

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
        pthread_mutex_unlock(&pm->mutex);
        return NULL;
    }
    for(int j = 0; j < num_paginas_proceso; j++) {
       uint32_t dir_pag_table = dir_primera_pagina + j;
       uint32_t dir_pag_libre = get_dir_pag_libre(pm);
       escribir_direccion_en_tabla_paginas(pm, dir_pag_table, dir_pag_libre);
    }

    pthread_mutex_unlock(&pm->mutex);
    return dir_primera_pagina;
}

void liberar_todas_las_direcciones_de_memoria_de_pagina(PhysicalMemory* pm, uint32_t dir_fisica_marco) {
    pthread_mutex_lock(&pm->mutex);

    for (int i = 0; i < TAMANIO_PAGINA; i++) {
        pm->memoria[dir_fisica_marco + i] = 0;
    }

    pthread_mutex_unlock(&pm->mutex);
}

void liberar_entrada_tabla_paginas(PhysicalMemory* pm, uint32_t dir_fisica_entrada_tabla_paginas) {
    pthread_mutex_lock(&pm->mutex);

    pm->memoria[dir_fisica_entrada_tabla_paginas] = 0;

    pthread_mutex_unlock(&pm->mutex);
}

void liberar_memoria_paginas(PhysicalMemory* pm, uint32_t dir_fisica_marco, int num_paginas) {
    uint32_t dir_primera_pagina_tabla;
    for (int i = 0; i < NUM_PAGINAS; i++) {
        if (pm->memoria[i] == dir_fisica_marco) {
            dir_primera_pagina_tabla = i;
            break;
        }
    }

    for (int i = 0; i < num_paginas; i++) {
        uint32_t dir_fisica_entrada_tabla_paginas = dir_primera_pagina_tabla + i;
        uint32_t dir_pag = pm->memoria[dir_fisica_entrada_tabla_paginas];
        liberar_todas_las_direcciones_de_memoria_de_pagina(pm, dir_pag);
        liberar_entrada_tabla_paginas(pm, dir_fisica_entrada_tabla_paginas);
    }
}

