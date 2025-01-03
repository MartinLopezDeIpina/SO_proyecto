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
#include "DebugPrints.h"


FILE* get_file_proceso(Loader* loader, int pid) {
    char id_proceso[4];
    char file_path[128];

    sprintf(id_proceso, "%03d", pid);
    strcpy(file_path, PROJECT_ROOT);
    strcat(file_path, "/prometheus/");
    strcat(file_path, "prog");
    strcat(file_path, id_proceso);
    strcat(file_path, ".elf");

    FILE* file = fopen(file_path, "r");
    return file;
}

void escribir_instrucciones_proceso_en_memoria_y_asignar_entradas_paginas(Loader* loader, PCB* pcb) {
    FILE* file = get_file_proceso(loader, loader -> ultimo_pid);
    // las dos primeras líneas no son instrucciones
    int num_instrucciones_totales;
    int* num_instrucciones_code = (int*)malloc(sizeof(int));
    int* num_instrucciones_data = (int*)malloc(sizeof(int));

    contar_instrucciones_code_y_data(file, num_instrucciones_code, num_instrucciones_data);;
    num_instrucciones_totales = *num_instrucciones_code + *num_instrucciones_data;

    char linea_header[14];    // ".text " + 6 hex + \n + \0
    char linea_dato[9];       // 8 hex + \0
    uint32_t text_addr, data_addr;

    // Leer dirección de text
    if (fgets(linea_header, sizeof(linea_header), file) != NULL) {
        sscanf(linea_header, ".text %d", &text_addr);
    }

    // Leer dirección de data
    if (fgets(linea_header, sizeof(linea_header), file) != NULL) {
        sscanf(linea_header, ".data %d", &data_addr);
    }

    int num_direcciones_code = data_addr - text_addr;
    // data_addr / tamanio_pagina redondeado hacia arriba
    int num_pags_text = (num_direcciones_code + TAMANIO_PAGINA - 1) / TAMANIO_PAGINA;

    int num_pags_data = (*num_instrucciones_data + TAMANIO_PAGINA - 1) / TAMANIO_PAGINA;

    int num_pags_totales = num_pags_text + num_pags_data;

    print_proceso_num_instrucciones(num_pags_text, num_pags_data, loader -> ultimo_pid);

    pcb->num_instrucciones = *num_instrucciones_code;

    *(pcb->mm_pcb->code) = text_addr;
    *(pcb->mm_pcb->data) = data_addr;

    uint32_t dir_primera_entrada_tabla_paginas = get_entrada_tabla_paginas_para_nuevo_proceso(loader->pm, num_pags_totales);
    *(pcb->mm_pcb->pgb) = dir_primera_entrada_tabla_paginas;

    int dir_logica_actual = text_addr;
    uint32_t dir_fisica_actual = get_valor_en_direccion_de_memoria(loader->pm, *pcb->mm_pcb->pgb);
    int indice_pagina_actual = 0;
    // Leer y escribir instrucciones de text
    if(pcb->pid == 4) {
        printf("debug");
    }
    for (int i = 0; i < num_direcciones_code; i++) {
        //Si se llega al final de la página, se pasa a la siguiente
        if(dir_logica_actual >= text_addr + TAMANIO_PAGINA * (indice_pagina_actual + 1)) {
            indice_pagina_actual++;
            int indice_tabla_paginas = dir_primera_entrada_tabla_paginas + indice_pagina_actual;
            dir_fisica_actual = get_valor_en_direccion_de_memoria(loader->pm, indice_tabla_paginas);
        }

        /*
        * Puede ser que se hayan escrito todas las instrucciones código pero haya que seguir reservando páginas porque algunos
        * procesos comienzan el segmento de datos varias páginas después
        */
        if (i < *num_instrucciones_code) {
            uint32_t valor;
            fscanf(file, "%X", &valor);
            escribir_valor_en_direccion(loader->pm, dir_fisica_actual, valor);
        }
        dir_logica_actual++;
        dir_fisica_actual++;
    }
    // Pasar a la siguiente página porque text y data están separados
    indice_pagina_actual++;
    int indice_pagina_codigo_actual = 0;
    int indice_tabla_paginas = dir_primera_entrada_tabla_paginas + indice_pagina_actual;
    dir_fisica_actual = get_valor_en_direccion_de_memoria(loader->pm, indice_tabla_paginas);
    for(int i = 0; i < *num_instrucciones_data; i++) {
        uint32_t valor;
        fscanf(file, "%X", &valor);

        //Si se llega al final de la página, se pasa a la siguiente
        if(dir_logica_actual >= data_addr + TAMANIO_PAGINA * (indice_pagina_codigo_actual + 1)) {
            indice_pagina_actual++;
            indice_pagina_codigo_actual++;
            int indice_tabla_paginas = dir_primera_entrada_tabla_paginas + indice_pagina_actual;
            dir_fisica_actual = get_valor_en_direccion_de_memoria(loader->pm, indice_tabla_paginas);
        }

        escribir_valor_en_direccion(loader->pm, dir_fisica_actual, valor);
        dir_logica_actual++;
        dir_fisica_actual++;
    }

    fclose(file);
}

void generar_proceso(Loader* loader) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB));

    int prioridad = int_aleatorio_entre_dos_numeros(1, 3);
    init_pcb(pcb, loader -> ultimo_pid, prioridad);

    escribir_instrucciones_proceso_en_memoria_y_asignar_entradas_paginas(loader, pcb);

    enqueue(loader -> process_queue, pcb);
    loader -> ultimo_pid++;
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
