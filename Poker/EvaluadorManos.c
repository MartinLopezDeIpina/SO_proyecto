//
// Created by martin on 12/27/24.
//

#include "EvaluadorManos.h"



/*
 * Funciones auxiliares para conteo y ordenamiento.
 */
static void contar_numeros(Carta* mano, int* resultados, int mano_size) {
    for(int i = 0; i < mano_size; i++) {
        resultados[mano[i].numero]++;
    }
}

static void contar_palos(Carta* mano, int* resultados, int mano_size) {
    for(int i = 0; i < mano_size; i++) {
        resultados[mano[i].palo]++;
    }
}

static int comparar_cartas(const void* a, const void* b) {
    const Carta* c1 = (const Carta*) a;
    const Carta* c2 = (const Carta*) b;
    return c1->numero - c2->numero;
}

/*
  Verifica la presencia de dos cartas iguales en número.
  Basta con encontrar un valor con frecuencia 2 o mayor.
 */
static Boolean contiene_pareja(Carta* todas, int mano_size) {
    int conteo[14] = {0};
    contar_numeros(todas, conteo, mano_size);
    for(int i = 1; i <= 13; i++) {
        if(conteo[i] >= 2) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
    Busca al menos dos valores diferentes que aparezcan en pares.
    Cuenta los valores para detectar si existen dos o más con frecuencia mayor o igual a 2.
 */
static Boolean contiene_doble_pareja(Carta* todas, int mano_size) {
    int conteo[14] = {0};
    contar_numeros(todas, conteo, mano_size);
    int pares_encontrados = 0;
    for(int i = 1; i <= 13; i++) {
        if(conteo[i] >= 2) {
            pares_encontrados++;
        }
    }
    return (pares_encontrados >= 2) ? TRUE : FALSE;
}

/*
    Determina si tres cartas tienen el mismo valor.
    Se fija en la frecuencia de cada número para ver si alguna alcanza 3.
 */
static Boolean contiene_trio(Carta* todas, int mano_size) {
    int conteo[14] = {0};
    contar_numeros(todas, conteo, mano_size);
    for(int i = 1; i <= 13; i++) {
        if(conteo[i] >= 3) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
Ordena las siete cartas por número y revisa si al menos cinco de ellas forman una secuencia consecutiva.
Reinicia el conteo al encontrar un salto en la numeración.
 */
static Boolean contiene_escalera(Carta* todas, int mano_size) {
    qsort(todas, mano_size, sizeof(Carta), comparar_cartas);
    int consecutivas = 1;
    for(int i = 1; i < mano_size; i++) {
        if(todas[i].numero == todas[i - 1].numero + 1) {
            consecutivas++;
        } else if(todas[i].numero != todas[i - 1].numero) {
            consecutivas = 1;
        }
        if(consecutivas >= 5) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
    Verifica si cinco o más cartas comparten el mismo palo.
    Se basa en contar cuántas cartas hay de cada uno de los cuatro palos.
 */
static Boolean contiene_color(Carta* todas, int mano_size) {
    int conteo_palos[4] = {0};
    contar_palos(todas, conteo_palos, mano_size);
    for(int i = 0; i < 4; i++) {
        if(conteo_palos[i] >= 5) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
    Revisa si hay cuatro cartas con el mismo número.
    Usa un conteo de valores para determinar si la frecuencia de alguno es 4.
 */
static Boolean contiene_poker(Carta* todas, int mano_size) {
    int conteo[14] = {0};
    contar_numeros(todas, conteo, mano_size);
    for(int i = 1; i <= 13; i++) {
        if(conteo[i] >= 4) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
    Comprueba si existe un trío y, simultáneamente, un par con otro valor.
    Combina la detección de un trío y un par en un único recuento.
 */
static Boolean contiene_full(Carta* todas, int mano_size) {
    int conteo[14] = {0};
    contar_numeros(todas, conteo, mano_size);
    Boolean hay_trio = FALSE, hay_par = FALSE;
    for(int i = 1; i <= 13; i++) {
        if(conteo[i] >= 3) hay_trio = TRUE;
        if(conteo[i] >= 2) hay_par = TRUE;
    }
    return (hay_trio && hay_par) ? TRUE : FALSE;
}

/*
    Verifica si, dentro de un mismo palo, hay 5 cartas consecutivas al ordenar por número.
    Esto combina la lógica de escalera y color a la vez.
 */
static Boolean contiene_escalera_color(Carta* todas, int mano_size) {
    // Requiere 5 consecutivos y mismo palo.
    // Primero verificar palos con >= 5 cartas, luego filtrar y comprobar si forman escalera.
    int conteo_palos[4] = {0};
    contar_palos(todas, conteo_palos, mano_size);

    for(int palo = 0; palo < 4; palo++) {
        if(conteo_palos[palo] >= 5) {
            // Filtrar solo cartas de ese palo
            Carta subset[mano_size];
            int idx = 0;
            for(int i = 0; i < mano_size; i++) {
                if(todas[i].palo == palo) {
                    subset[idx++] = todas[i];
                }
            }
            // Ordenar
            qsort(subset, idx, sizeof(Carta), comparar_cartas);

            int consecutivas = 1;
            for(int j = 1; j < idx; j++) {
                if(subset[j].numero == subset[j - 1].numero + 1) {
                    consecutivas++;
                } else if(subset[j].numero != subset[j - 1].numero) {
                    consecutivas = 1;
                }
                if(consecutivas >= 5) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/*
    Comprueba si existen las cartas 10, Jota, Reina, Rey y As dentro del mismo palo.
    Solo devuelve TRUE cuando están todas ellas juntas en la misma mano.
 */
static Boolean contiene_escalera_real(Carta* todas, int mano_size) {
    // Necesita: 10, J, Q, K, A (numeros 10,11,12,13,1) del mismo palo
    for(int palo = 0; palo < 4; palo++) {
        Boolean tiene10 = FALSE, tieneJ = FALSE, tieneQ = FALSE;
        Boolean tieneK = FALSE, tieneA = FALSE;
        for(int i = 0; i < mano_size; i++) {
            if(todas[i].palo == palo) {
                if(todas[i].numero == DIEZ)  tiene10 = TRUE;
                if(todas[i].numero == JOTA)  tieneJ = TRUE;
                if(todas[i].numero == REINA) tieneQ = TRUE;
                if(todas[i].numero == REY)   tieneK = TRUE;
                if(todas[i].numero == AS)    tieneA = TRUE;
            }
        }
        if(tiene10 && tieneJ && tieneQ && tieneK && tieneA) {
            return TRUE;
        }
    }
    return FALSE;
}

static Boolean contiene_carta_alta(Carta* todas, int mano_size) {
    // "Carta alta" se considera verdadero si ninguna otra combinación aplica.
    // En la práctica, se retorna TRUE porque siempre hay al menos una carta.
    return TRUE;
}

static void combinar_cartas(Carta* propias, Carta* comunes, int mano_size_total, Carta* resultado) {
    int mano_size = mano_size_total - 5;
    for(int i = 0; i < mano_size; i++){
        resultado[i] = propias[i];
    }
    for(int j = 0; j < 5; j++){
        resultado[j + mano_size] = comunes[j];
    }
}

Boolean mano_contiene_combinacion(Carta* todas, enum Combinacion combinacion, int mano_size) {

    switch(combinacion) {
        case ESCALERA_REAL:
            return contiene_escalera_real(todas, mano_size);
        case ESCALERA_COLOR:
            return contiene_escalera_color(todas, mano_size);
        case POKER:
            return contiene_poker(todas, mano_size);
        case FULL:
            return contiene_full(todas, mano_size);
        case COLOR:
            return contiene_color(todas, mano_size);
        case ESCALERA:
            return contiene_escalera(todas, mano_size);
        case TRIO:
            return contiene_trio(todas, mano_size);
        case DOBLE_PAREJA:
            return contiene_doble_pareja(todas, mano_size);
        case PAREJA:
            return contiene_pareja(todas, mano_size);
        case CARTA_ALTA:
            return contiene_carta_alta(todas, mano_size);
        default:
            return FALSE;
    }
}

int valor_mano(Carta* todas, int mano_size) {
    for(enum Combinacion combinacion = ESCALERA_REAL; combinacion >= CARTA_ALTA; combinacion--) {
        Boolean contiene_combinacion = mano_contiene_combinacion(todas, combinacion, mano_size);
        if(contiene_combinacion == TRUE) {
            return combinacion;
        }
    }
    return -1;
}

/*
 * Comparación para ordenar de mayor a menor por número.
 */
static int comparar_desc(const void* a, const void* b) {
    const Carta* c1 = (const Carta*)a;
    const Carta* c2 = (const Carta*)b;
    // Orden descendente por número
    return c2->numero - c1->numero;
}

/*
 * Desempate simplificado: se comparan las 7 cartas de cada jugador
 * Gana quien tenga la primera carta mayor; si todas son iguales, gana B arbitrariamente.
 */
static Boolean desempate_por_cartas(Carta* a_todas, Carta* b_todas, int mano_size_a, int mano_size_b) {
    // Ordenar ambas manos de mayor a menor
    qsort(a_todas, mano_size_a, sizeof(Carta), comparar_desc);
    qsort(b_todas, mano_size_b, sizeof(Carta), comparar_desc);

    int min_mano_size = (mano_size_a < mano_size_b) ? mano_size_a : mano_size_b;

    // Comparar carta a carta hasta encontrar una diferencia
    for(int i = 0; i < min_mano_size; i++){
        if(a_todas[i].numero > b_todas[i].numero) {
            return TRUE;  // Jugador A tiene una carta mayor
        } else if(a_todas[i].numero < b_todas[i].numero) {
            return FALSE; // Jugador B tiene la carta mayor
        }
    }
    if (min_mano_size < mano_size_a) {
        return TRUE; // A tiene más cartas, gana
    }else if(min_mano_size > mano_size_a) {
        return FALSE; // B tiene más cartas, gana
    }
    // Si todas las cartas son iguales en valor, A no tiene mejor mano.
    return FALSE;
}

/*
 * - Compara primero los valores globales de la mano (valor_mano).
 * - Si son iguales, aplica un desempate simplificado por valor de cartas.
 */
Boolean mano_jugador_a_mejor_mano_jugador_b(PCB* a, PCB* b, Carta* cartas_comunes) {
    int mano_size_a = a -> prioridad + 5;
    int mano_size_b = b -> prioridad + 5;

    Carta a_todas[mano_size_a], b_todas[mano_size_b];
    combinar_cartas(a->cartas, cartas_comunes, mano_size_a, a_todas);
    combinar_cartas(b->cartas, cartas_comunes, mano_size_b, b_todas);

    int valor_a = valor_mano(a_todas, mano_size_a);
    int valor_b = valor_mano(b_todas, mano_size_b);

    // Si la puntuación de A es mayor, gana directamente
    if(valor_a > valor_b) {
        return TRUE;
    }
    // Si la puntuación de B es mayor, A no gana
    if(valor_a < valor_b) {
        return FALSE;
    }

    return desempate_por_cartas(a_todas, b_todas, mano_size_a, mano_size_b);
}