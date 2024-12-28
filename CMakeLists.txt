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
        ProcessGenerator.h
        ProcessQueue.c
        Boolean.h
        ProcessQueue.h
        Scheduler.c
        Scheduler.h
        Machine/Machine.c
        Machine/Machine.h
        Machine/CPU.c
        Machine/CPU.h
        Machine/Core.c
        Machine/Core.h
        utils.c
        utils.h
        Poker/Carta.h
        Poker/Baraja.c
        Poker/Carta.c
        Poker/Baraja.h
        Poker/Partida.c
        Poker/Partida.h
        Poker/EvaluadorManos.c
        Poker/EvaluadorManos.h
        Poker/SklanskyMalmuth.h
        Poker/SklanskyMalmuth.c
        Poker/PokerUtils.c
        Poker/PokerUtils.h
)
