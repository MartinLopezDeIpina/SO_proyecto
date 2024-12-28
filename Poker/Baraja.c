//
// Created by martin on 12/27/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../utils.h"
#include "Baraja.h"

void init_baraja(Baraja* baraja, int num_cartas_necesarias_para_jugadores) {

    // 5 cartas comunitarias
    int cartas_necesarias = num_cartas_necesarias_para_jugadores + 5;

    // Redondea al siguiente múltiplo de 52
    int num_barajas = (cartas_necesarias + 51) / 52;
    baraja -> num_barajas = num_barajas;

    int cartas_baraja = num_barajas * 52;
    baraja->num_cartas = cartas_baraja;
    baraja->cartas = (Carta*)malloc(sizeof(Carta) * cartas_baraja);

    int indice = 0;
    for (int b = 0; b < num_barajas; b++) {
        for (enum Palo palo = CORAZONES; palo <= PICAS; palo++) {
            for (enum Numero numero = AS; numero <= REY; numero++) {
                baraja->cartas[indice].palo = palo;
                baraja->cartas[indice].numero = numero;
                indice++;
            }
        }
    }
}

void shuffle_baraja(Baraja* baraja) {
    //iniciar semilla de nums aleatorios
    srand(time(NULL));

    // Algoritmo Fisher-Yates shuffle
    for (int i = baraja->num_cartas - 1; i > 0; i--) {
        // Generar un índice aleatorio entre 0 e i
        int j = int_aleatorio_entre_dos_numeros(0, i);

        // Intercambiar las cartas en las posiciones i y j
        Carta temp = baraja->cartas[i];
        baraja->cartas[i] = baraja->cartas[j];
        baraja->cartas[j] = temp;
    }
}

Carta repartir_carta(Baraja* baraja) {
    Carta* carta = &baraja->cartas[baraja->num_cartas - 1];
    baraja->num_cartas--;

    return *carta;
}

void print_baraja(Baraja* baraja) {
    for (int i = 0; i < baraja->num_cartas; i++) {
        char* carta_str = carta_to_string(&baraja->cartas[i]);
        printf("%s ", carta_str);
    }
}
