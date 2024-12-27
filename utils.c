//
// Created by martin on 12/25/24.
//

#include <stdlib.h>

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