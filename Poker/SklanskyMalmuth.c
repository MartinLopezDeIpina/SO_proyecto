//
// Created by martin on 12/28/24.
//
#include "SklanskyMalmuth.h"

/*
 * En teoría sería imposible 2 cartas iguales del mismo palo, pero puede que se repartan variasa barajas porque hay demasiados procesos.
 */
int get_valor_sklansky_malmuth_suited(Carta carta1, Carta carta2) {
   switch(carta1.numero) {
      case AS:
         switch(carta2.numero) {
            case AS: return 1;
            case REY: return 1;
            case REINA: return 2;
            case JOTA: return 2;
            case DIEZ: return 3;
            default: return 5;
         }
      case REY:
         switch (carta2.numero) {
            case REY: return 1;
            case REINA: return 2;
            case JOTA: return 3;
            case DIEZ: return 4;
            default: return 7;
         }
      case REINA:
         switch (carta2.numero) {
            case REINA: return 1;
            case JOTA: return 3;
            case DIEZ: return 4;
            case NUEVE: return 5;
            case OCHO: return 7;
            default: return 9;
         }
      case JOTA:
         switch (carta2.numero) {
            case JOTA: return 1;
            case DIEZ: return 3;
            case NUEVE: return 4;
            case OCHO: return 6;
            case SIETE: return 6;
            default: return 9;
         }
      case DIEZ:
         switch (carta2.numero) {
            case DIEZ: return 2;
            case NUEVE: return 4;
            case OCHO: return 5;
            case SIETE: return 7;
            default: return 9;
         }
      case NUEVE:
         switch (carta2.numero) {
            case NUEVE: return 3;
            case OCHO: return 4;
            case SIETE: return 5;
            case SEIS: return 8;
            default: return 9;
         }
      case OCHO:
         switch (carta2.numero) {
            case OCHO: return 4;
            case SIETE: return 5;
            case SEIS: return 6;
            case CINCO: return 8;
            default: return 9;
         }
      case SIETE:
         switch (carta2.numero) {
            case SIETE: return 5;
            case SEIS: return 5;
            case CINCO: return 6;
            case CUATRO: return 8;
            default: return 9;
         }
      case SEIS:
         switch (carta2.numero) {
            case SEIS: return 5;
            case CINCO: return 6;
            case CUATRO: return 7;
            default: return 9;
         }
      case CINCO:
         switch (carta2.numero) {
            case CINCO: return 6;
            case CUATRO: return 6;
            case TRES: return 7;
            default: return 9;
         }
      case CUATRO:
         switch (carta2.numero) {
            case CUATRO: return 7;
            case TRES: return 7;
            case DOS: return 8;
            default: return 9;
         }
      case TRES:
         switch (carta2.numero) {
            case TRES: return 7;
            case DOS: return 8;
            default: return 9;
         }
      case DOS:
         switch (carta2.numero) {
            case DOS: return 7;
            default: return 9;
         }
      default:
         return 9;
   }
}

int get_valor_sklansky_malmuth_offsuited(Carta carta1, Carta carta2) {
  switch(carta1.numero) {
     case AS:
        switch(carta2.numero) {
           case AS: return 1;
           case REY: return 1;
           default: return 9;
        }
     case REY:
        switch (carta2.numero) {
           case AS: return 2;
           case REY: return 1;
           default: return 9;
        }
     case REINA:
        switch (carta2.numero) {
           case AS: return 3;
           case REY: return 4;
           case REINA: return 1;
           default: return 9;
        }
     case JOTA:
        switch (carta2.numero) {
           case AS: return 4;
           case REY: return 5;
           case REINA: return 5;
           case JOTA: return 1;
           default: return 9;
        }
     case DIEZ:
        switch (carta2.numero) {
           case AS: return 6;
           case REY: return 6;
           case REINA: return 6;
           case JOTA: return 5;
           case DIEZ: return 2;
           default: return 9;
        }
     case NUEVE:
        switch (carta2.numero) {
           case AS: return 8;
           case REY: return 8;
           case REINA: return 8;
           case JOTA: return 7;
           case DIEZ: return 7;
           case NUEVE: return 3;
           default: return 9;
        }
     case OCHO:
        switch (carta2.numero) {
           case JOTA: return 8;
           case DIEZ: return 8;
           case NUEVE: return 7;
           case OCHO: return 4;
           default: return 9;
        }
     case SIETE:
        switch (carta2.numero) {
           case OCHO: return 8;
           case SIETE: return 5;
           default: return 9;
        }
     case SEIS:
        switch (carta2.numero) {
           case SIETE: return 8;
           case SEIS: return 5;
           default: return 9;
        }
     case CINCO:
        switch (carta2.numero) {
           case SEIS: return 8;
           case CINCO: return 6;
           default: return 9;
        }
     case CUATRO:
        switch (carta2.numero) {
           case CINCO: return 8;
           case CUATRO: return 7;
           default: return 9;
        }
     case TRES:
        switch (carta2.numero) {
           case TRES: return 7;
           default: return 9;
        }
     case DOS:
        switch (carta2.numero) {
           case DOS: return 7;
           default: return 9;
        }
     default:
        return 9;
  }
}

/*
 * En caso de los procesos no prioritarios que solo tienen una carta, devolver un valor arbitrario.
 */
int get_valor_arbitrario_una_carta(Carta carta) {
   switch(carta.numero) {
      case AS: return 1;
      case REY: return 2;
      case REINA: return 3;
      case JOTA: return 4;
      case DIEZ: return 6;
      case NUEVE: return 8;
      default: return 9;
   }
}

int get_valor_sklansky_malmuth_suited_offsuited(Carta carta1, Carta carta2) {
   if (carta1.palo == carta2.palo) {
      return get_valor_sklansky_malmuth_suited(carta1, carta2);
   }else {
      return get_valor_sklansky_malmuth_offsuited(carta1, carta2);
   }
}

int get_valor_sklansky_malmuth_2(Carta carta1, Carta carta2) {

   int valor = get_valor_sklansky_malmuth_suited_offsuited(carta1, carta2);
   // En caso de ser la peor puntuación, puede que sea porque las cartas están al revés, probar con la otra combinación
   if (valor == 9){
      valor = get_valor_sklansky_malmuth_suited_offsuited(carta2, carta1);
   }
   return valor;
}

/*
 * En caso de los procesos prioritarios con 3 cartas, devolver la combinación de 2 cartas con mayor valor.
 */
int get_valor_sklansky_malmuth_3(Carta* mano) {
   Carta carta1 = mano[0];
   Carta carta2 = mano[1];
   Carta carta3 = mano[2];

   int valor1 = get_valor_sklansky_malmuth_2(carta1, carta2);
   int valor2 = get_valor_sklansky_malmuth_2(carta1, carta3);
   int valor3 = get_valor_sklansky_malmuth_2(carta2, carta3);

   int max = valor1;
   if (valor2 > max) {
      max = valor2;
   }
   if (valor3 > max) {
      max = valor3;
   }
   return max;
}

int get_valor_sklansky_malmuth(Carta* mano, int num_cartas) {
   if(num_cartas == 2) {
      return get_valor_sklansky_malmuth_2(mano[0], mano[1]);
   }
   if(num_cartas == 1) {
      return get_valor_arbitrario_una_carta(mano[0]);
   }
   if(num_cartas == 3) {
      return get_valor_sklansky_malmuth_3(mano);
   }
   return 9;
}