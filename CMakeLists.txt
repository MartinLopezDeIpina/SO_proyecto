cmake_minimum_required(VERSION 3.29)
project(untitled C)


add_executable(untitled main.c
        Clock.c
        Semaforo.c
        Semaforo.h
        Clock.h
        Temporizador.h
        Temporizador.c
        PCB.c
        PCB.h
        ProcessGenerator.c
        IComponenteTemporizable.h
)
