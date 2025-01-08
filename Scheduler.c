//
// Created by martin on 11/3/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"
#include "Machine/Machine.h"
#include "Poker/Partida.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    ProcessQueue* process_queue;
    Machine* machine;
}Scheduler;

PCB* jugar_partida_poker_y_get_ganador(Scheduler* scheduler, PCBArray* pcb_candidatos, int id_core) {
    Partida partida;
    init_partida(&partida, pcb_candidatos, id_core);
    PCB* ganador = jugar_partida_poker(&partida);
    return ganador;
}

void asignar_procesos_a_hilos_ociosos(Scheduler* scheduler) {
    PCBArray* pcb_candidatos = get_procesos_candidatos_partida_poker(scheduler->process_queue);
    int num_hilos_ociosos = get_num_hilos_ociosos_machine(scheduler->machine);

    // en caso de que haya menos procesos que cores ociosos
    if(pcb_candidatos != NULL && pcb_candidatos->cantidad <= num_hilos_ociosos) {
        for(int i = 0; i < pcb_candidatos->cantidad; i++) {
            asignar_saldo_ejecucion(pcb_candidatos->pcbs[i]);
            int id_core_a_asignar = get_id_primer_core_ocioso_machine(scheduler->machine);
            // Nunca ha dado -1 pero si lo da es que hay algún problema, acabar la función para no crashear la ejecución.
            if(id_core_a_asignar == -1) {
                printf("Error, no hay cores ociosos\n");
                break;
            }
            asignar_proceso_a_machine(scheduler->machine, id_core_a_asignar, pcb_candidatos->pcbs[i]);
        }
    }else {
        for (int i = 0; i < num_hilos_ociosos; i++) {
            pcb_candidatos = get_procesos_candidatos_partida_poker(scheduler->process_queue);
            if (pcb_candidatos != NULL) {
                int id_core_a_asignar = get_id_primer_core_ocioso_machine(scheduler->machine);
                if(id_core_a_asignar == -1) {
                    printf("Error, no hay cores ociosos\n");
                    break;
                }
                PCB* ganador;
                if (UTILIZAR_POKER_SCHEDULER == TRUE) {
                    ganador = jugar_partida_poker_y_get_ganador(scheduler, pcb_candidatos, id_core_a_asignar);
                }else {
                    ganador = get_proceso_listo_cola_fifo(scheduler->process_queue);
                }
                asignar_saldo_ejecucion(ganador);
                asignar_proceso_a_machine(scheduler->machine, id_core_a_asignar, ganador);
            }
        }
    }
}


void funcion_scheduler(Scheduler* scheduler) {
    printf("ejeutando funcion scheduler\n");

    incrementar_saldos_fuera_cpu(scheduler->process_queue);

    vaciar_cores_sin_saldo_suficiente(scheduler->machine);

    eliminar_procesos_terminados(scheduler->process_queue);

    asignar_procesos_a_hilos_ociosos(scheduler);

    print_queue(scheduler -> process_queue);
}

void ejecutar_funcion_temporizador_scheduler(void* self) {
    Scheduler* scheduler = (Scheduler*)self;
    funcion_scheduler(scheduler);
}

void init_scheduler(Scheduler* scheduler, ProcessQueue* process_queue, Machine* machine) {
    scheduler -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_scheduler;
    scheduler -> process_queue = process_queue;
    scheduler -> machine = machine;
}
