//
// Created by martin on 12/23/24.
//

#include <stdio.h>
#include <pthread.h>
#include "Core.h"

/*
 * El objeto Core puede ser accedido por más de un hilo, por ejemplo el scheduler para asignar y quitar procesos, y el clock para notificar el tick.
 * El mutex mutex_acceso_core es para proteger el acceso al core.
 * El mutex mutex es para sincronizar la ejecución de instrucciones con los ticks del reloj.
 */

void ejecutar_instruccion(Core* core) {
    pthread_mutex_lock(&core->mutex_acceso_core);
    ejecutar_instruccion_proceso(core -> current_process);
    pthread_mutex_unlock(&core->mutex_acceso_core);
}

void print_estado_core(Core* core) {
    pthread_mutex_lock(&core->mutex_acceso_core);

    printf("Core %d: ", core -> id_core);
    if (core -> current_process) {
        printear_instrucciones_ejecutadas(core -> current_process);
    } else {
        printf("Ocioso\n");
    }

    pthread_mutex_unlock(&core->mutex_acceso_core);
}

void* funcion_core(void* arg) {
    Core* core = (Core*)arg;

    while(1) {
        pthread_mutex_lock(&core->mutex);
        pthread_cond_wait(&core->condition, &core->mutex);

        Boolean core_ocioso = core_esta_ocioso(core);
        if (core_ocioso == FALSE) {
            ejecutar_instruccion(core);
        }

        pthread_mutex_unlock(&core->mutex);
    }
    return NULL;
}

void init_core(int id_core, Core* core) {
    core -> current_process = NULL;
    core -> id_core = id_core;

    pthread_mutex_init(&core->mutex_acceso_core, NULL);
    pthread_create(&core->thread, NULL, funcion_core, (void*)core);
}

void notificar_tick_clock_core(Core* core) {
    pthread_mutex_lock(&core->mutex);
    print_estado_core(core);
    pthread_cond_signal(&core->condition);
    pthread_mutex_unlock(&core->mutex);
}

Boolean core_esta_vacio(Core* core) {
    pthread_mutex_lock(&core->mutex_acceso_core);

    int core_esta_vacio = core -> current_process == NULL;

    pthread_mutex_unlock(&core->mutex_acceso_core);

    if(core_esta_vacio) {
        return TRUE;
    }else {
        return FALSE;
    }
}

// no usar el mutex porque ya lo usan las funciones de dentro
Boolean core_esta_ocioso(Core* core) {
    int core_esta_ocioso = core_esta_vacio(core) || proceso_ha_terminado(core -> current_process);

    if(core_esta_ocioso) {
        return TRUE;
    }else {
        return FALSE;
    }

}

Boolean proceso_core_ha_terminado(Core* core) {
    pthread_mutex_lock(&core->mutex_acceso_core);
    int ha_terminado = proceso_ha_terminado(core -> current_process);
    pthread_mutex_unlock(&core->mutex_acceso_core);

    if(ha_terminado) {
        return TRUE;
    }else {
        return FALSE;
    }
}

Boolean proceso_core_saldo_ejecucion_insuficiente(Core* core) {
    pthread_mutex_lock(&core->mutex_acceso_core);
    Boolean saldo_insuficiente = proceso_saldo_ejecucion_insuficiente(core -> current_process);
    pthread_mutex_unlock(&core->mutex_acceso_core);

    return saldo_insuficiente;
}

void vaciar_core(Core* core) {
    pthread_mutex_lock(&core->mutex_acceso_core);
    core -> current_process = NULL;
    pthread_mutex_unlock(&core->mutex_acceso_core);
}

void vaciar_core_y_set_estado(Core* core, EstadoProceso estado) {
    pthread_mutex_lock(&core->mutex_acceso_core);
    set_estado_proceso(core -> current_process, estado);
    core -> current_process = NULL;
    pthread_mutex_unlock(&core->mutex_acceso_core);
}

void asignar_proceso_a_core(Core* core, PCB* pcb) {
    pthread_mutex_lock(&core->mutex_acceso_core);
    core -> current_process = pcb;
    set_estado_proceso_ejecutando(pcb);
    pthread_mutex_unlock(&core->mutex_acceso_core);
}