//
// Created by martin on 12/28/24.
//

#include "Carta.h"
#include "SklanskyMalmuth.h"
#include "PokerUtils.h"

#include <string.h>

#include "EvaluadorManos.h"
#include "../globals.h"
#include "../DebugPrints.h"

// small blind es 1 porque todos pagan 1 para jugar la partida.
int BIG_BLIND = 2;
int SMALL_BLIND = 1;

float get_pot_odds(int pot, int apuesta_necesaria) {
    return (float)apuesta_necesaria / (float)(pot + apuesta_necesaria);
}

/*
* Fórmula completamente inventada
* Si tenemos el valor_s 1 y hay 5 jugadores, nos da 80 de equidad.
*/
float get_equidad_preflop_con_sklansky_malmuth(Carta** mano, int cantidad_cartas, int cantidad_jugadores) {
   int valor_s = get_valor_sklansky_malmuth(mano, cantidad_cartas);

   float equidad = 100.0f - (float)(valor_s * 4 * cantidad_jugadores);

   return equidad;
}

// Función auxiliar para verificar si una carta ya está en uso
static Boolean carta_esta_en_uso(Carta* carta, Carta** cartas_usadas, int num_cartas_usadas) {
    for(int i = 0; i < num_cartas_usadas; i++) {
        if(cartas_usadas[i]->numero == carta->numero && cartas_usadas[i]->palo == carta->palo) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * Primero calcular la equidad contra un oponente con 2 cartas y dividirlo por el número de jugadores
 */
float get_equidad_postflop(Carta** cartas_conocidas, int cantidad_cartas_conocidas, int cantidad_jugadores) {
    Carta* cartas_disponibles = (Carta*)malloc(sizeof(Carta) * 52);
    int num_cartas_disponibles = 0;

    // Llenar el array de cartas disponibles
    for(int palo = 0; palo < 4; palo++) {
        for(int numero = 1; numero <= 13; numero++) {

            Carta carta = cartas_disponibles[num_cartas_disponibles];
            carta.palo = palo;
            carta.numero = numero;

            if(carta_esta_en_uso(&carta, cartas_conocidas, cantidad_cartas_conocidas) == FALSE) {
                cartas_disponibles[num_cartas_disponibles++] = carta;
            }
        }
    }

    int victorias = 0;
    int total_manos = 0;

    int num_cartas_mano = cantidad_cartas_conocidas - 5;

    // Obtener las cartas comunes -> las primeras son las de la mano del jugador y las otras 5 son las comunes
    Carta* cartas_comunes = (Carta*)malloc(sizeof(Carta) * 5);
    for (int i = 0; i < 5; i++) {
        cartas_comunes[i] = *cartas_conocidas[i + num_cartas_mano];
    }

    Carta* cartas_mano = (Carta*)malloc(sizeof(Carta) * num_cartas_mano);
    for (int i = 0; i < num_cartas_mano; i++) {
        cartas_mano[i] = *cartas_conocidas[i];
    }

    Carta* cartas_oponente = (Carta*)malloc(sizeof(Carta) * 2);

    // Para cada posible combinación de 2 cartas del oponente
    for(int i = 0; i < num_cartas_disponibles - 1; i++) {
        cartas_oponente[0] = cartas_disponibles[i];

        for(int j = i + 1; j < num_cartas_disponibles; j++) {
            cartas_oponente[1] = cartas_disponibles[j];

            total_manos++;
            printf("probando oponente con %s %s\n", carta_to_string(&cartas_oponente[0]), carta_to_string(&cartas_oponente[1]));
            if(mano_a_mejor_mano_b(cartas_mano, cartas_oponente,  num_cartas_mano, 2, cartas_comunes) == TRUE) {
                victorias++;
            }
        }
    }
    float equidad = (float)victorias / (float)total_manos;

    free(cartas_disponibles);
    return equidad / (float)cantidad_jugadores;
}

/*
 * Si tenemos 0.8 de equidad, el pot es de 10 y la apuesta es de 3 -> ev = 7.6, entonces relacion con BB = 3.7 -> apostar x 3
 */
int get_dinero_a_apostar(Carta** cartas_conocidas, int num_cartas, int pot, int apuesta_necesaria, int num_jugadores, Boolean preflop) {

    if (apuesta_necesaria == 0) {
        apuesta_necesaria = SMALL_BLIND;
    }

    float equidad;
    /* Solo tenemos nuestras 2 cartas conocidas -> (50 2) x (48 5) ~= 2 millones de combinaciones para la mano del rival.
     * Demasiadas combinaciones para calcular en el scheduler, mejor usar la tabla de Sklansky-Malmuth para obtener el valor de la mano
     * Si tenemos 5 cartas conocidas, hay que calcular las combinaciones para las 2 cartas del rival
     */
    if (preflop == TRUE) {
        equidad = get_equidad_preflop_con_sklansky_malmuth(cartas_conocidas, num_cartas, num_jugadores);
    }else {
        equidad = get_equidad_postflop(cartas_conocidas, num_cartas, num_jugadores);
    }

    float p_ganar = equidad / 100;
    float p_perder = (100 - equidad)/100;
    float ganancia = (float)pot;
    float perdida = (float)apuesta_necesaria;

    float expected_value = (p_ganar * ganancia) - (p_perder * perdida);

    float relacion = expected_value / (float)BIG_BLIND;

    print_info_apuesta(equidad, relacion);


    if(relacion >= 2) {
        return apuesta_necesaria * 3;
    }
    if (relacion >= 1.5) {
        return apuesta_necesaria * 2;
    }
    if (relacion >= 0.5) {
        return apuesta_necesaria;
    }
    return 0;
}




