//
// Created by martin on 10/29/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IComponenteTemporizable.h"
#include "ProcessQueue.h"
#include "globals.h"
#include "Loader.h"


FILE* get_file_proceso(Loader* loader, int pid) {
    char id_proceso[4];
    char file_path[128];

    sprintf(id_proceso, "%03d", pid);
    strcpy(file_path, PATH_FICHEROS_PROCESOS);
    strcat(file_path, "prog");
    strcat(file_path, id_proceso);
    strcat(file_path, ".elf");

    FILE* file = fopen(file_path, "r");
    return file;
}

void escribir_instrucciones_proceso_en_memoria_y_asignar_entradas_paginas(Loader* loader, PCB* pcb) {
    FILE* file = get_file_proceso(loader, loader -> ultimo_pid + 1);
    // las dos primeras líneas no son instrucciones
    int num_instrucciones = contar_lineas_fichero(file) - 2;

    char linea_header[14];    // ".text " + 6 hex + \n + \0
    char linea_dato[9];       // 8 hex + \0
    uint32_t text_addr, data_addr;

    // Leer dirección de text
    if (fgets(linea_header, sizeof(linea_header), file) != NULL) {
        sscanf(linea_header, ".text %X", &text_addr);
    }

    // Leer dirección de data
    if (fgets(linea_header, sizeof(linea_header), file) != NULL) {
        sscanf(linea_header, ".data %X", &data_addr);
    }

    // data_addr / tamanio_pagina redondeado hacia arriba
    int num_pags_text = (data_addr + TAMANIO_PAGINA - 1) / TAMANIO_PAGINA;

    int num_instrucciones_data = num_instrucciones - data_addr / TAMANIO_PAGINA;
    int num_pags_data = (num_instrucciones_data + TAMANIO_PAGINA - 1) / TAMANIO_PAGINA;

    int num_pags_totales = num_pags_text + num_pags_data;

    pcb->mm_pcb->code = (uint32_t*)text_addr;
    pcb->mm_pcb->data = (uint32_t*)data_addr;

    int32_t* dir_primera_entrada_tabla_paginas = get_entrada_tabla_paginas_para_nuevo_proceso(loader->pm, num_pags_totales);

    pcb->mm_pcb->pgb = dir_primera_entrada_tabla_paginas;

    int dir_logica_actual = text_addr;
    int32_t dir_fisica_actual = (int32_t)malloc(sizeof(int32_t));
    dir_fisica_actual = *dir_primera_entrada_tabla_paginas;
    int indice_pagina_actual = 0;
    // Leer y escribir instrucciones de text
    for (int i = 0; i < num_pags_text; i++) {
        uint32_t valor;
        fgets(linea_dato, sizeof(linea_dato), file);
        sscanf(linea_dato, "%X", &valor);

        loader->pm->memoria[dir_fisica_actual] = valor;
        dir_logica_actual++;
        dir_fisica_actual++;
        //Si se llega al final de la página, se pasa a la siguiente
        if(dir_logica_actual >= TAMANIO_PAGINA * (indice_pagina_actual + 1)) {
            indice_pagina_actual++;
            dir_fisica_actual = *dir_primera_entrada_tabla_paginas + indice_pagina_actual;
        }
    }
    // Pasar a la siguiente página porque text y data están separados
    indice_pagina_actual++;
    dir_fisica_actual = *dir_primera_entrada_tabla_paginas + indice_pagina_actual;
    for(int i = 0; i < num_pags_data; i++) {
        uint32_t valor;
        fgets(linea_dato, sizeof(linea_dato), file);
        sscanf(linea_dato, "%X", &valor);

        loader->pm->memoria[dir_fisica_actual] = valor;
        dir_logica_actual++;
        dir_fisica_actual++;
        //Si se llega al final de la página, se pasa a la siguiente
        if(dir_logica_actual >= data_addr + TAMANIO_PAGINA * (indice_pagina_actual + 1)) {
            indice_pagina_actual++;
            dir_fisica_actual = *dir_primera_entrada_tabla_paginas + indice_pagina_actual;
        }
    }

    fclose(file);
}

void generar_proceso(Loader* loader) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB));

    int prioridad = int_aleatorio_entre_dos_numeros(1, 3);
    init_pcb(pcb, loader -> ultimo_pid, prioridad);
    loader -> ultimo_pid++;

    escribir_instrucciones_proceso_en_memoria_y_asignar_entradas_paginas(loader, pcb);

    enqueue(loader -> process_queue, pcb);
}

void ejecutar_funcion_temporizador_process_generator(void* self) {
    Loader* process_generator = (Loader*)self;

    generar_proceso(process_generator);
}

void init_loader(Loader* process_generator, ProcessQueue* process_queue, PhysicalMemory* pm) {
    process_generator -> i_componente_temporizable.ejecutar_funcion_temporizador = ejecutar_funcion_temporizador_process_generator;
    process_generator -> process_queue = process_queue;
    process_generator -> ultimo_pid = 0;
    process_generator -> pm = pm;

    for(int i = 0; i < NUM_PROCESOS_INICIALES; i++) {
        generar_proceso(process_generator);
    }
}
