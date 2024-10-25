#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    int value;
} semaphore;

void semaphore_init(semaphore* sem, int initial_value);
void semaphore_wait(semaphore* sem);
void semaphore_signal(semaphore* sem);

#endif