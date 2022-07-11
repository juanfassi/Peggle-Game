/*
Contiene toda la fisica del juego.
*/
#ifndef FISICA_H
#define FISICA_H

#include <stdbool.h>

#include "lista_obstaculos.h"
#include "config.h"

double computar_velocidad(double vi, double a, double dt);

double computar_posicion(double pi, double vi, double dt);

bool buscar_rebote(float *cx, float *cy,float *vx,float *vy,lista_t *lista, size_t *score, size_t *naranja, int *contador_pelota,bool* contacto);

float producto_interno(float ax, float ay, float bx, float by);

void punto_mas_cercano(float x0, float y0, float x1, float y1, float xp, float yp, float *x, float *y);

void reflejar(float norm_x, float norm_y, float *cx, float *cy, float *vx, float *vy);

double poligono_distancia(const poligono_t *p, float xp, float yp, float *nor_x, float *nor_y);

double distancia(float xa, float ya, float xb, float yb);

void calcular_pos_bola(poligono_t *bola, float radio, float centro_x, float centro_y);


#endif