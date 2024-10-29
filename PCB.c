//
// Created by martin on 10/29/24.
//

typedef struct {
    int pid;
} PCB;

void init_pcb(PCB* pcb, int pid) {
    pcb -> pid = pid;
}