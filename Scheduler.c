//
// Created by martin on 11/3/24.
//

#include <stdio.h>
#include <stdlib.h>

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
    printf("jugando partida de poker\n");
    Partida* partida;
    init_partida(partida, pcb_candidatos, id_core);
    PCB* ganador = jugar_partida_poker(partida);
    return ganador;
}

void asignar_procesos_a_cores_ociosos(Scheduler* scheduler, int* ids_cores_ociosos, int num_cores_ociosos) {
    for (int i = 0; i < num_cores_ociosos; i++) {
        PCBArray* pcb_candidatos = get_procesos_candidatos_partida_poker(scheduler->process_queue);
        if (pcb_candidatos != NULL) {
            PCB* ganador = jugar_partida_poker_y_get_ganador(scheduler, pcb_candidatos, ids_cores_ociosos[i]);
            asignar_saldo_ejecucion(ganador);
            asignar_proceso_a_machine(scheduler->machine, ids_cores_ociosos[i], ganador);
        }
    }
}


void funcion_scheduler(Scheduler* scheduler) {
    printf("ejeutando funcion scheduler\n");

    incrementar_saldos_fuera_cpu(scheduler->process_queue);

    int* pid_procesos_terminados = (int*)malloc(get_num_cores_machine(scheduler->machine) * sizeof(int));
    int num_procesos_terminados = vaciar_cores_terminados(scheduler->machine, pid_procesos_terminados);

    vaciar_cores_sin_saldo_suficiente(scheduler->machine);

    eliminar_procesos_terminados(scheduler->process_queue);

    int* ids_cores_ociosos = (int*)malloc(get_num_cores_machine(scheduler->machine) * sizeof(int));
    int num_cores_ociosos = get_ids_cores_ociosos(scheduler->machine, ids_cores_ociosos);

    asignar_procesos_a_cores_ociosos(scheduler, ids_cores_ociosos, num_cores_ociosos);

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
