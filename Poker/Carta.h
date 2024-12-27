//
// Created by martin on 12/27/24.
//

#ifndef CARTA_H
#define CARTA_H


enum Palo {
    CORAZONES,
    DIAMANTES,
    TREBOLES,
    PICAS
};
enum Numero {
    AS = 1,
    DOS = 2,
    TRES = 3,
    CUATRO = 4,
    CINCO = 5,
    SEIS = 6,
    SIETE = 7,
    OCHO = 8,
    NUEVE = 9,
    DIEZ = 10,
    JOTA = 11,
    REINA = 12,
    REY = 13
};

enum Combinacion {
    CARTA_ALTA = 0,
    PAREJA = 1,
    DOBLE_PAREJA = 2,
    TRIO = 3,
    ESCALERA = 4,
    COLOR = 5,
    FULL = 6,
    POKER = 7,
    ESCALERA_COLOR = 8,
    ESCALERA_REAL = 9
};

typedef struct {
    enum Palo palo;
    enum Numero numero;
}Carta;

char* palo_a_string(enum Palo palo);
char* carta_to_string(Carta* carta);


#endif //CARTA_H
