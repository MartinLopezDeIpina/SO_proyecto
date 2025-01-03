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

/**
 *Va iterando en la memoria física de página en página, mirando si la dirección de la página está en la tabla de páginas.
 *Función interna sin mutex, llamarla solo si se tiene el mutex.
 */
uint32_t get_dir_pag_libre(PhysicalMemory* pm) {
    for (int i = 0; i < NUM_DIRECCIONES_PAGINAS; i++) {
        uint32_t dir_pag = DIR_PRIMERA_PAGINA + i * TAMANIO_PAGINA;
        // buscar si la tabla de páginas tiene la dirección de la página.
        for(int j = 0; j < NUM_PAGINAS; j++) {
            // j * 4 porque cada dirección de página ocupa 4 bytes.
            if (leer_palabra_de_memoria(pm, j * 4) == dir_pag) {
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
    escribir_palabra_en_memoria(pm, dir_pag_tabla_paginas, dir_pag);
    print_direccion_escrita(dir_pag_tabla_paginas, leer_palabra_de_memoria(pm, dir_pag_tabla_paginas));
}

void escribir_direccion_en_tabla_paginas(PhysicalMemory* pm, uint32_t dir_pag_tabla_paginas, uint32_t dir_pag) {
    pthread_mutex_lock(&pm->mutex);

    escribir_direccion_en_tabla_paginas_sin_mutex(pm, dir_pag_tabla_paginas, dir_pag);

    pthread_mutex_unlock(&pm->mutex);
}

void escribir_valor_en_direccion(PhysicalMemory* pm, uint32_t dir, uint32_t valor) {
    pthread_mutex_lock(&pm->mutex);

    escribir_palabra_en_memoria(pm, dir, valor);
    print_direccion_escrita(dir, leer_palabra_de_memoria(pm, dir));

    pthread_mutex_unlock(&pm->mutex);
}

/*
 * Dada una dirección de memoria, devuelve el valor en memoria usando la dirección como índice.
 */
uint32_t get_valor_en_direccion_de_memoria(PhysicalMemory* pm, uint32_t dir) {
    pthread_mutex_lock(&pm->mutex);

    uint32_t valor = leer_palabra_de_memoria(pm, dir);

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
           if (leer_palabra_de_memoria(pm, (i+j)*4) != 0) {
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
       uint32_t dir_pag_table = (dir_primera_pagina + j) * 4;
       uint32_t dir_pag_libre = get_dir_pag_libre(pm);
       escribir_direccion_en_tabla_paginas_sin_mutex(pm, dir_pag_table, dir_pag_libre);
    }

    pthread_mutex_unlock(&pm->mutex);
    // convertir dirección lógica de la página a dirección de entrada en la tabla de páginas.
    return dir_primera_pagina * 4;
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

    escribir_palabra_en_memoria(pm, dir_fisica_entrada_tabla_paginas, 0);

    pthread_mutex_unlock(&pm->mutex);
}

void liberar_memoria_paginas(PhysicalMemory* pm, uint32_t dir_fisica_marco, int num_paginas) {
    uint32_t dir_primera_pagina_tabla;
    for (int i = 0; i < NUM_PAGINAS; i++) {
        if (leer_palabra_de_memoria(pm, i*4) == dir_fisica_marco) {
            //dirección física de tabla de páginas
            dir_primera_pagina_tabla = i*4;
            break;
        }
    }

    for (int i = 0; i < num_paginas; i++) {
        uint32_t dir_fisica_entrada_tabla_paginas = dir_primera_pagina_tabla + (i * 4);
        uint32_t dir_pag = leer_palabra_de_memoria(pm, dir_fisica_entrada_tabla_paginas);
        liberar_todas_las_direcciones_de_memoria_de_pagina(pm, dir_pag);
        liberar_entrada_tabla_paginas(pm, dir_fisica_entrada_tabla_paginas);
    }
}

