//
// Created by martin on 11/3/24.
//

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "PCB.h"
#include "Boolean.h"

typedef struct Node{
   PCB* pcb;
   struct Node* next;
}Node;

Node* create_node(PCB* pcb) {
   //todo: gestionar el leak de memoria
   Node* node = (Node*)malloc(sizeof(Node));
   node -> pcb = pcb;
   node -> next = NULL;
   return node;
}

typedef struct {
   Node* primer_nodo;
   Node* ultimo_nodo;
}ProcessQueue;

Boolean is_empty(ProcessQueue* process_queue) {
   return process_queue -> primer_nodo == NULL;
}

void init_process_queue(ProcessQueue* process_queue) {
   process_queue -> primer_nodo = NULL;
   process_queue -> ultimo_nodo = NULL;
}

void enqueue(ProcessQueue* queue, PCB* pcb){
   Node* new_node = create_node(pcb);

   if (is_empty(queue) == TRUE) {
      queue -> primer_nodo = queue -> ultimo_nodo = new_node;
      return;
   }

   queue -> ultimo_nodo -> next = new_node;
   queue -> ultimo_nodo = new_node;
}

void dequeue(ProcessQueue* queue){
    if (is_empty(queue)) {
        printf("Queue Underflow\n");
        return;
    }

    Node* temp = queue -> primer_nodo;
    queue -> primer_nodo = queue -> primer_nodo -> next;

    // If front becomes null, then change rear also to null
    if (queue->primer_nodo == NULL)
        queue->ultimo_nodo = NULL;

    free(temp);
}

PCB* get_primero(ProcessQueue *queue)
{

    if (is_empty(queue)) {
        printf("Queue is empty\n");
        return NULL;
    }
    return queue-> primer_nodo -> pcb;
}

PCB* get_ultimo(ProcessQueue* queue)
{

    if (is_empty(queue)) {
        printf("Queue is empty\n");
        return NULL;
    }
    return queue-> ultimo_nodo -> pcb;
}

void print_queue(ProcessQueue* queue) {
    Node* current = queue -> primer_nodo;
    while (current != NULL) {
        printf("%d ", current -> pcb -> pid);
        current = current -> next;
    }
    printf("\n");
}

