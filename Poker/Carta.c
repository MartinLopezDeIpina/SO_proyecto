//
// Created by martin on 12/27/24.
//

#include "Carta.h"
#include <stdio.h>
#include <stdlib.h>

const char* numero_a_string(enum Numero numero) {
    switch (numero) {
        case AS: return "A";
        case JOTA: return "J";
        case REINA: return "Q";
        case REY: return "K";
        default: return NULL;  // Para números del 2-10
    }
}

char* palo_a_string(enum Palo palo) {
    switch (palo) {
        case CORAZONES: return "♥";
        case DIAMANTES: return "♦";
        case TREBOLES: return "♣";
        case PICAS: return "♠";
        default: return "?";
    }
}

char* carta_to_string(Carta* carta) {
    char* string = (char*)malloc(50 * sizeof(char));
    const char* palo_str = palo_a_string(carta->palo);
    const char* num_str = numero_a_string(carta->numero);

    if (num_str != NULL) {
        // Es AS, J, Q o K
        sprintf(string, "%s%s", num_str, palo_str);
    } else {
        // Es un número del 2-10
        sprintf(string, "%d%s", carta->numero, palo_str);
    }

    return string;
}
