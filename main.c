#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "Clock.h"
#include "Temporizador.h"

void temporizador_funcion() {
    printf("Temporizador haciendo algo\n");
}

int main(void) {
    Clock clock;
    init_clock(&clock, 1, 2);

    Temporizador temp1;
    Temporizador temp2;

    init_temporizador(&temp1, temporizador_funcion, &clock);
    init_temporizador(&temp2, temporizador_funcion, &clock);

    sleep(1000);
    return 0;
}
