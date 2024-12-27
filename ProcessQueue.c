//
// Created by martin on 11/3/24.
//

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "PCB.h"
#include "Boolean.h"
#include "utils.h"
#include <pthread.h>


typedef struct Node {
    PCB* pcb;
    struct Node* next;
} Node;

Node* create_node(PCB* pcb) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->pcb = pcb;
    node->next = NULL;
    return node;
}

/*
 * Esta lista puede ser accedida por más de un hilo, por ejemplo el process generator y el scheduler, debe ser thread safe.
 */

typedef struct {
    Node* primer_nodo;
    Node* ultimo_nodo;
    pthread_mutex_t mutex;
} ProcessQueue;

Boolean is_empty(ProcessQueue* process_queue) {
    return (process_queue->primer_nodo == NULL);
}

void init_process_queue(ProcessQueue* process_queue) {
    process_queue->primer_nodo = NULL;
    process_queue->ultimo_nodo = NULL;
    pthread_mutex_init(&process_queue->mutex, NULL);
}

void enqueue(ProcessQueue* queue, PCB* pcb) {
    pthread_mutex_lock(&queue->mutex);

    Node* new_node = create_node(pcb);
    if (is_empty(queue) == TRUE) {
        queue->primer_nodo = queue->ultimo_nodo = new_node;
    } else {
        queue->ultimo_nodo->next = new_node;
        queue->ultimo_nodo = new_node;
    }

    pthread_mutex_unlock(&queue->mutex);
}

void dequeue(ProcessQueue* queue) {
    pthread_mutex_lock(&queue->mutex);

    if (is_empty(queue)) {
        printf("Queue Underflow\n");
        pthread_mutex_unlock(&queue->mutex);
        return;
    }

    Node* temp = queue->primer_nodo;
    queue->primer_nodo = queue->primer_nodo->next;

    if (queue->primer_nodo == NULL) {
        queue->ultimo_nodo = NULL;
    }

    free(temp);

    pthread_mutex_unlock(&queue->mutex);
}

void eliminar_procesos_terminados(ProcessQueue* queue) {
    pthread_mutex_lock(&queue->mutex);

    Node* current = queue->primer_nodo;
    Node* prev = NULL;

    while (current != NULL) {
        if (proceso_ha_terminado(current->pcb)) {
            Node* temp = current;

            // Ajustar los punteros para eliminar el nodo
            if (prev == NULL) {
                queue->primer_nodo = current->next;
            } else {
                prev->next = current->next;
            }

            // Actualizar el último nodo si es necesario
            if (current == queue->ultimo_nodo) {
                queue->ultimo_nodo = prev;
            }

            current = current->next;
            free(temp);
        } else {
            prev = current;
            current = current->next;
        }
    }

    pthread_mutex_unlock(&queue->mutex);
}

void incrementar_saldos_fuera_cpu(ProcessQueue* queue) {
    pthread_mutex_lock(&queue->mutex);

    Node* current = queue->primer_nodo;
    while (current != NULL) {
        if(proceso_esta_listo(current->pcb)) {
            incrementar_saldo(current->pcb, 1);
        }
        current = current->next;
    }

    pthread_mutex_unlock(&queue->mutex);
}

Node* get_primero(ProcessQueue* queue) {
    if (is_empty(queue)) {
        printf("Queue is empty\n");
        return NULL;
    }
    Node* first = queue->primer_nodo;

    return first;
}

Node* get_ultimo(ProcessQueue* queue) {
    if (is_empty(queue)) {
        printf("Queue is empty\n");
        return NULL;
    }
    Node* last = queue->ultimo_nodo;

    return last;
}

void print_queue(ProcessQueue* queue) {
    pthread_mutex_lock(&queue->mutex);

    Node* current = queue->primer_nodo;
    while (current != NULL) {
        printf("%d(%d$) ", current->pcb->pid, current->pcb->saldo);
        current = current->next;
    }
    printf("\n");

    pthread_mutex_unlock(&queue->mutex);
}

void lock_queue_mutex(ProcessQueue* queue) {
    pthread_mutex_lock(&queue->mutex);
}
void unlock_queue_mutex(ProcessQueue* queue) {
    pthread_mutex_unlock(&queue->mutex);
}

int get_num_procesos_en_cola(ProcessQueue* queue) {

    int num_procesos = 0;
    Node* current = queue->primer_nodo;
    while (current != NULL) {
        num_procesos++;
        current = current->next;
    }

    return num_procesos;
}

PCBArray* get_procesos_candidatos_partida_poker(ProcessQueue* queue) {
    pthread_mutex_lock(&queue->mutex);

    int tamano_cola = get_num_procesos_en_cola(queue);

    PCBArray* resultado = (PCBArray*)malloc(sizeof(PCBArray));
    resultado->pcbs = (PCB**)malloc(sizeof(PCB*) * tamano_cola);
    resultado->cantidad = 0;

    // Recorrer la cola y añadir los PCBs que cumplan los criterios
    Node* current = queue->primer_nodo;
    while (current != NULL) {
        if (proceso_esta_listo(current->pcb) && proceso_saldo_suficiente_para_entrar_core(current->pcb)) {
            resultado->pcbs[resultado->cantidad] = current->pcb;
            resultado->cantidad++;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&queue->mutex);

    // Si no encontramos ningún PCB válido, liberamos la memoria y devolvemos NULL
    if (resultado->cantidad == 0) {
        free(resultado->pcbs);
        free(resultado);
        return NULL;
    }

    return resultado;
}