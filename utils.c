//
// Created by martin on 12/25/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Boolean.h"

// para ordenar de forma ascendente
int comparar_enteros(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

float float_aleatorio_entre_dos_numeros(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

int int_aleatorio_entre_dos_numeros(int min, int max) {
    return min + rand() % (max - min + 1);
}

Boolean evento_con_probabilidad(float probabilidad) {

    float numero_aleatorio = (float)rand() / (float)RAND_MAX;

    return (numero_aleatorio < probabilidad) ? TRUE : FALSE;
}

void contar_instrucciones_code_y_data(FILE* file, int* num_instrucciones_code, int* num_instrucciones_data) {
    *num_instrucciones_code = 0;
    *num_instrucciones_data = 0;
    char linea[16];

    // Ignorar las dos primeras líneas (.text y .data)
    fgets(linea, sizeof(linea), file);
    fgets(linea, sizeof(linea), file);

    // Contar líneas código -> hasta encontrar F0000000 (instrucción exit)
    while (fgets(linea, sizeof(linea), file) != NULL) {
        if (strncmp(linea, "F", 1) == 0) {  // Si la línea empieza con F
            (*num_instrucciones_code)++;
            break;
        }
        (*num_instrucciones_code)++;
    }

    // Contar líneas de datos
    while (fgets(linea, sizeof(linea), file) != NULL) {
        (*num_instrucciones_data)++;
    }

    //volver al inicio del archivo
    fseek(file, 0, SEEK_SET);
}
