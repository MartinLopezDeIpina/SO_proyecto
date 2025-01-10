//
// Created by martin on 12/29/24.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include "Boolean.h"

#define DEBUG_PARTIDA TRUE
#define DEBUG_DIRS_MEMORIA TRUE
#define SALDO_NECESARIO_PARA_JUGAR_POKER 5
#define INSTRUCCIONES_POR_PROCESO 10
#define UTILIZAR_POKER_SCHEDULER TRUE
#define NUM_PROCESOS_INICIALES 1

#define NUM_BYTES_DIRECCION 3

// 2^24 = 16777216
#define NUM_DIRECCIONES 16777216

// en celdas de 1 byte
#define TAMANIO_PAGINA 12

#define NUM_PAGINAS (NUM_DIRECCIONES / TAMANIO_PAGINA)

// Quitar el espacio de las páginas de la tabla de páginas. -> num páginas * 4 es lo que ocupa la tabla de páginas.
#define NUM_DIRECCIONES_PAGINAS ((NUM_DIRECCIONES - (NUM_PAGINAS * NUM_BYTES_DIRECCION)) / TAMANIO_PAGINA)

// Si hay 1048756 páginas, entonces a 4 direcciones por página, la tabla de páginas ocupa 4194304 bytes, lo que deja 12582912 bytes para lo demás.
#define DIR_PRIMERA_PAGINA NUM_PAGINAS * NUM_BYTES_DIRECCION

// size en cantidad de entradas
#define SIZE_TLB 16

#define PATH_FICHEROS_PROCESOS "prometheus/"

#define MAX_PROCESOS 50

#endif //GLOBALS_H
