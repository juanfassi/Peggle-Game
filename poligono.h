#ifndef POLIGONO_H
#define POLIGONO_H

#include <stddef.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define RES_BOLA 0.3

struct poligono{
    float (*vertices)[2];
    size_t n;
};

typedef struct poligono poligono_t;

typedef enum{
    AZUL,
    NARANJA,
    VERDE,
    GRIS,
    BLANCO, //este color es el que se va a eliminar
    PURPURA
} color_t;

poligono_t *poligono_crear(float vertices[][2], size_t n);

poligono_t *generar_bola(float radio, float centro_x, float centro_y);

void poligono_destruir(poligono_t *p);


#endif
