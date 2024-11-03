//
// Created by martin on 11/3/24.
//

#ifndef PROCESSQUEUE_H
#define PROCESSQUEUE_H
#include "Boolean.h"
#include "PCB.h"

typedef struct {
    PCB* pcb;
    struct Node* next;
}ProcessQueue;

Boolean is_empty(ProcessQueue* process_queue);
void init_process_queue(ProcessQueue* process_queue);
void enqueue(ProcessQueue* queue, PCB* pcb);
void dequeue(ProcessQueue* queue);

PCB* get_primero(ProcessQueue* queue);
PCB* get_ultimo(ProcessQueue* queue);

void print_queue(ProcessQueue* queue);


#endif //PROCESSQUEUE_H
