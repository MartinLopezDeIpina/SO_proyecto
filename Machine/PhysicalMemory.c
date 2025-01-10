//
// Created by martin on 12/30/24.
//

#include "PhysicalMemory.h"

#include <stdio.h>

#include "../globals.h"

#include <stdlib.h>
#include <pthread.h>

#include "../DebugPrints.h"

void init_physical_memory(PhysicalMemory* pm) {
    pm->memoria = (uint8_t*)malloc(NUM_DIRECCIONES * sizeof(uint8_t));
    for (int i = 0; i < NUM_DIRECCIONES; i++) {
        pm->memoria[i] = 0;
    }
    pthread_mutex_init(&pm->mutex, NULL);
}

/*
 * Función para escribir una palabra de 4 bytes (una instrucción) en 4 direcciones de memoria.
 */
static void escribir_palabra_en_memoria(PhysicalMemory* pm, uint32_t dir, uint32_t valor) {
    pm->memoria[dir] = (valor >> 24) & 0xFF;
    pm->memoria[dir + 1] = (valor >> 16) & 0xFF;
    pm->memoria[dir + 2] = (valor >> 8) & 0xFF;
    pm->memoria[dir + 3] = valor & 0xFF;
}

/*
 * Función auxiliar para leer una palabra de 32 bits de memoria byte a byte
 */
static uint32_t leer_palabra_de_memoria(PhysicalMemory* pm, uint32_t dir) {
    return ((uint32_t)pm->memoria[dir] << 24) |
           ((uint32_t)pm->memoria[dir + 1] << 16) |
           ((uint32_t)pm->memoria[dir + 2] << 8) |
           ((uint32_t)pm->memoria[dir + 3]);
}

/*
 * Función para escribir una dirección de memoria de NUM_BYTES_DIRECCION bytes en NUM_BYTES_DIRECCION direcciones de memoria.
 */
static void escribir_direccion_en_memoria(PhysicalMemory* pm, uint32_t dir, uint32_t valor) {
    for (int i = 0; i < NUM_BYTES_DIRECCION; i++) {
        /*
         * Con direcciones de 3 bytes, cuando i = 0 se desplaza 16 bits -> solo quedan 8 bits más significativos, porque la dirección está en los 3 bytes más significativos.
         * cuando i = 1 se desplaza 8 bits -> solo quedan 16 bits más significativos
         */
        pm->memoria[dir + i] = (valor >> (8 * (NUM_BYTES_DIRECCION - i - 1))) & 0xFF;
    }
}

/*
 * Función para leer dirección en memoria de NUM_BYTES_DIRECCION bytes.
 */
static uint32_t leer_direccion_de_memoria(PhysicalMemory* pm, uint32_t dir) {
    uint32_t direccion = 0;
    for (int i = 0; i < NUM_BYTES_DIRECCION; i++) {
        direccion |= ((uint32_t)pm->memoria[dir + i]) << (8 * (NUM_BYTES_DIRECCION - i - 1));
    }

    return direccion;
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
            // como j es el índice de la página, se busca la dirección en la tabla de páginas con el tamaño de una dirección.
            if (leer_direccion_de_memoria(pm, j * NUM_BYTES_DIRECCION) == dir_pag) {
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

uint32_t get_dir_pag_libre_pm(PhysicalMemory* pm) {
    pthread_mutex_lock(&pm->mutex);

    uint32_t dir_pag = get_dir_pag_libre(pm);

    pthread_mutex_unlock(&pm->mutex);
    return dir_pag;
}

void escribir_direccion_en_tabla_paginas_sin_mutex(PhysicalMemory* pm, uint32_t dir_pag_tabla_paginas, uint32_t dir_pag) {
    escribir_direccion_en_memoria(pm, dir_pag_tabla_paginas, dir_pag);
    print_direccion_escrita(dir_pag_tabla_paginas, leer_direccion_de_memoria(pm, dir_pag_tabla_paginas));
}

void escribir_direccion_en_tabla_paginas(PhysicalMemory* pm, uint32_t dir_pag_tabla_paginas, uint32_t dir_pag) {
    pthread_mutex_lock(&pm->mutex);

    escribir_direccion_en_tabla_paginas_sin_mutex(pm, dir_pag_tabla_paginas, dir_pag);

    pthread_mutex_unlock(&pm->mutex);
}

// Si es una dirección de tabla de páginas, entonces se tendrá que escribir el tamaño de una dirección, sino 4 bytes (una palabra).
void escribir_valor_en_direccion(PhysicalMemory* pm, uint32_t dir, uint32_t valor, Boolean es_direccion_tabla_paginas) {
    pthread_mutex_lock(&pm->mutex);

    if (es_direccion_tabla_paginas == TRUE) {
        escribir_direccion_en_memoria(pm, dir, valor);
    }else {
        escribir_palabra_en_memoria(pm, dir, valor);
    }
    print_direccion_escrita(dir, valor);

    pthread_mutex_unlock(&pm->mutex);
}

/*
 * Dada una dirección de memoria, devuelve el valor en memoria usando la dirección como índice.
 * Solo para buscar valores en la tabla de páginas
 */
uint32_t get_valor_en_direccion_de_memoria(PhysicalMemory* pm, uint32_t dir, Boolean es_direccion_tabla_paginas) {
    pthread_mutex_lock(&pm->mutex);

    uint32_t valor = -1;
    if(es_direccion_tabla_paginas == TRUE) {
        valor = leer_direccion_de_memoria(pm, dir);
    }else {
        valor = leer_palabra_de_memoria(pm, dir);
    }

    pthread_mutex_unlock(&pm->mutex);
    return valor;
}

//Devuelve la dirección de memoria donde se encuentra la entrada a la primera página del nuevo proceso.
uint32_t get_entrada_tabla_paginas_para_nuevo_proceso(PhysicalMemory* pm, int num_paginas_proceso) {
    pthread_mutex_lock(&pm->mutex);

    Boolean encontrado = FALSE;
    // Se busca la primera entrada libre en la tabla de páginas.
    uint32_t dir_primera_pagina = 0;
    for(int i = 0; i < NUM_PAGINAS; i++) {
       //comprobar si num_paginas_proceso están libres
       dir_primera_pagina = i;
       for(int j = 0; j < num_paginas_proceso; j++) {
           if (leer_direccion_de_memoria(pm, (i+j)*NUM_BYTES_DIRECCION) != 0) {
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
       // convertir dirección lógica de la página a dirección de entrada en la tabla de páginas.
       uint32_t dir_pag_table = (dir_primera_pagina + j) * NUM_BYTES_DIRECCION;
       uint32_t dir_pag_libre = get_dir_pag_libre(pm);
       escribir_direccion_en_tabla_paginas_sin_mutex(pm, dir_pag_table, dir_pag_libre);
    }

    pthread_mutex_unlock(&pm->mutex);
    // convertir dirección lógica de la página a dirección de entrada en la tabla de páginas.
    return dir_primera_pagina * NUM_BYTES_DIRECCION;
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

    escribir_direccion_en_memoria(pm, dir_fisica_entrada_tabla_paginas, 0);

    pthread_mutex_unlock(&pm->mutex);
}

void liberar_memoria_paginas(PhysicalMemory* pm, uint32_t dir_fisica_marco, int num_paginas) {
    uint32_t dir_primera_pagina_tabla = dir_fisica_marco;

    for (int i = 0; i < num_paginas; i++) {
        uint32_t dir_fisica_entrada_tabla_paginas = dir_primera_pagina_tabla + (i * NUM_BYTES_DIRECCION);
        uint32_t dir_pag = leer_direccion_de_memoria(pm, dir_fisica_entrada_tabla_paginas);
        liberar_todas_las_direcciones_de_memoria_de_pagina(pm, dir_pag);
        liberar_entrada_tabla_paginas(pm, dir_fisica_entrada_tabla_paginas);
    }
}

