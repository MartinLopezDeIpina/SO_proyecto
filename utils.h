//
// Created by martin on 12/25/24.
//

#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

#include "Boolean.h"

#endif //UTILS_H

int comparar_enteros(const void *a, const void *b);
float float_aleatorio_entre_dos_numeros(float min, float max);
int int_aleatorio_entre_dos_numeros(int min, int max);
Boolean evento_con_probabilidad(float probabilidad);
void contar_instrucciones_code_y_data(FILE* file, int* num_instrucciones_code, int* num_instrucciones_data);
