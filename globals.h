//
// Created by martin on 12/29/24.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include "Boolean.h"

#define debug_partida FALSE
#define SALDO_NECESARIO_PARA_JUGAR_POKER 5
#define INSTRUCCIONES_POR_PROCESO 10
#define UTILIZAR_POKER_SCHEDULER TRUE
#define NUM_PROCESOS_INICIALES 15

// 2^24 = 16777216
#define NUM_DIRECCIONES 16777216

// en celdas de 4 bytes
#define TAMANIO_PAGINA 4

// 4096 x 4096 = 16777216
#define NUM_PAGINAS (NUM_DIRECCIONES / TAMANIO_PAGINA)
// Quitar el espacio de las páginas de la tabla de páginas. -> num páginas * 4 es lo que ocupa la tabla de páginas.
#define NUM_DIRECCIONES_PAGINAS (NUM_DIRECCIONES - NUM_PAGINAS)/TAMANIO_PAGINA
// 1000 es 4096 en hexadecimal, dir de primera página física después de tabla de páginas.
#define DIR_PRIMERA_PAGINA 0x1000

#define SIZE_TLB 16

#define PATH_FICHEROS_PROCESOS "prometheus/"

#endif //GLOBALS_H
