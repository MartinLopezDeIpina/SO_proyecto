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

// 4096 * 1024 * 4 = 16777216
static int TAMANIO_PAGINA = 4096;
static int NUM_PAGINAS = 1024;
// 1000 es 4096 en hexadecimal, dir de primera página física después de tabla de páginas.
__uint32_t DIR_PRIMERA_PAGINA = 0x1000;

int SIZE_TLB = 16;

#endif //GLOBALS_H
