//
// Created by martin on 10/29/24.
//

#ifndef ICOMPONENTETEMPORIZABLE_H
#define ICOMPONENTETEMPORIZABLE_H

typedef struct {
   void (*ejecutar_funcion_temporizador)(void* self);
}IComponenteTemporizable;

#endif //ICOMPONENTETEMPORIZABLE_H
