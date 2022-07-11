#ifndef LISTA_OBSTACULOS_H
#define LISTA_OBSTACULOS_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "poligono.h"

#define MASK_COLOR 0xC0
#define SHIFT_COLOR 6
#define MASK_MOV 0x30
#define SHIFT_MOV 4
#define MASK_GEOM 0xF

#define PARAMETRO_GEO_CIRCULO 3
#define PARAMETRO_GEO_RECTANGULO 5

typedef enum movimiento{
    MOV_INMOVIL,
    MOV_CIRCULAR,
    MOV_HORIZONTAL
}movimiento_t;

struct obstaculo{
    poligono_t* forma;
    movimiento_t movimiento;
    float parametros[3];
    color_t color;
};

typedef struct obstaculo obstaculo_t;

struct nodo {
    struct nodo *sig;
    obstaculo_t *dato; 
};

struct lista {
    struct nodo *prim;
};

typedef struct lista lista_t;


typedef enum{
    GEO_CIRCULO,
    GEO_RECTANGULO,
    GEO_POLIGONO
} geometria_t;


poligono_t *leer_geometria_circulo(FILE *f);

poligono_t *leer_geometria_rectangulo(FILE *f);

poligono_t *leer_geometria_poligono(FILE *f);

poligono_t *leer_geometria(FILE*f, geometria_t geometria);

obstaculo_t*obstaculo_crear_desde_archivo(FILE*f);

void obstaculo_destruir(obstaculo_t*obs);

obstaculo_t**obstaculo_leer_archivo(FILE*f,size_t *n_obstaculos);

lista_t *lista_crear();

void lista_destruir(lista_t *l);

bool lista_es_vacia(const lista_t *l);

void *lista_primero(const lista_t *l);

bool lista_insertar_al_final(lista_t *l, obstaculo_t *dato);

int lista_largo(struct nodo *l);

void lista_mover(const lista_t *l);

bool mover(obstaculo_t* m);

bool generar_movimiento_inmovil(obstaculo_t* m);

bool generar_movimiento_circular(obstaculo_t* m);

bool generar_movimiento_horizontal(obstaculo_t* m);

void rotar(float poligono[][2], size_t n, double rad);

void trasladar (float poligono[][2], size_t n, float dx, float dy);

void rotar_respecto_centro(poligono_t *poligono,float angulo,float centro_x, float centro_y);

bool leer_movimiento_inmovil(FILE *f, int16_t parametros[], size_t *n_parametros);

bool leer_movimiento_circular(FILE *f, int16_t parametros[], size_t *n_parametros);

bool leer_movimiento_horizontal(FILE *f, int16_t parametros[], size_t *n_parametros);

bool leer_movimiento(FILE *f, movimiento_t movimiento, int16_t parametros[], size_t *n_parametros);

bool leer_encabezado(FILE *f, color_t *color, movimiento_t *movimiento, geometria_t *geometria);


void lista_borrar_elemento(lista_t *l, color_t eliminar, size_t * obstaculos);

lista_t * lista_crear_desde_archivo(FILE *f, size_t*obs);

#endif