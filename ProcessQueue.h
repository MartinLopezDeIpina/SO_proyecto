//
// Created by martin on 11/3/24.
//

#ifndef PROCESSQUEUE_H
#define PROCESSQUEUE_H
#include "Boolean.h"
#include "PCB.h"

typedef struct Node{
   PCB* pcb;
   struct Node* next;
}Node;

typedef struct {
    Node* primer_nodo;
    Node* ultimo_nodo;
    pthread_mutex_t mutex;
} ProcessQueue;


Boolean is_empty(ProcessQueue* process_queue);
void init_process_queue(ProcessQueue* process_queue);
void enqueue(ProcessQueue* queue, PCB* pcb);
void dequeue(ProcessQueue* queue);
void eliminar_procesos_terminados(ProcessQueue* queue);
void lock_queue_mutex(ProcessQueue* queue);
void unlock_queue_mutex(ProcessQueue* queue);
void incrementar_saldos_fuera_cpu(ProcessQueue* queue);
PCBArray* get_procesos_candidatos_partida_poker(ProcessQueue* queue);

Node* get_primero(ProcessQueue* queue);
Node* get_ultimo(ProcessQueue* queue);

void print_queue(ProcessQueue* queue);


#endif //PROCESSQUEUE_H
