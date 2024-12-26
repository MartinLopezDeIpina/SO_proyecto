//
// Created by martin on 11/3/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"
#include "Machine/Machine.h"

typedef struct {
    IComponenteTemporizable i_componente_temporizable;
    ProcessQueue* process_queue;
    Machine* machine;
}Scheduler;


void asignar_procesos_a_cores_ociosos(Scheduler* scheduler, int* ids_cores_ociosos, int num_cores_ociosos) {
    lock_queue_mutex(scheduler->process_queue);

    Node* nodo_actual = get_primero(scheduler->process_queue);
    int i = 0;
    while (nodo_actual != NULL && i < num_cores_ociosos) {
        PCB* pcb = nodo_actual->pcb;
        if (pcb->estado == LISTO && proceso_saldo_suficiente_para_entrar_core(pcb)) {
            asignar_saldo_ejecucion(pcb);
            asignar_proceso_a_machine(scheduler->machine, ids_cores_ociosos[i], pcb);
            i++;
        }
        nodo_actual = nodo_actual->next;
    }

    unlock_queue_mutex(scheduler->process_queue);
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
