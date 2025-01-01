//
// Created by martin on 12/29/24.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include "Boolean.h"

static Boolean debug_partida = FALSE;
static int SALDO_NECESARIO_PARA_JUGAR_POKER = 5;
static int INSTRUCCIONES_POR_PROCESO = 10;
static Boolean UTILIZAR_POKER_SCHEDULER = TRUE;
static Boolean NUM_PROCESOS_INICIALES = 15;

// 2^24 = 16777216
static int NUM_DIRECCIONES = 16777216;

// en celdas de 4 bytes
static int TAMANIO_PAGINA = 4096;

// 4096 x 4096 = 16777216
static int NUM_PAGINAS = 4096;
// 1000 es 4096 en hexadecimal, dir de primera página física después de tabla de páginas.
static __uint32_t DIR_PRIMERA_PAGINA = 0x1000;

static int SIZE_TLB = 16;

static char* PATH_FICHEROS_PROCESOS = "prometheus/";

#endif //GLOBALS_H
