//
// Created by martin on 10/29/24.
//

#ifndef PCB_H
#define PCB_H

typedef struct {
    int pid;
} PCB;

void init_pcb(PCB* pcb, int pid);

#endif //PCB_H
