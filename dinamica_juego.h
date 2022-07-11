/*
Marca la dinamica del juego:
    lee el encabezado del .bin
    como se pasa de nivel.
    decide si la pelota esta trabada para que se eliminen los obstaculos.
*/

#ifndef DINAMICA_JUEGO_H
#define DINAMICA_JUEGO_H

#include <stdbool.h>

#include "lista_obstaculos.h"

bool es_trabada(size_t cont_tiempo, float cx, float cy, float cx_ref, float cy_ref);

void obstaculo_sumar(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota);

void suma_azul(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota);

void suma_verde(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota);

void suma_naranja(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota);

bool gana_nivel(lista_t *l);

size_t cant_naranja(lista_t *lista);

void reiniciar_nivel(int *contador_pelotas,int * nivel,int bits_movidos,FILE*f);

bool obstaculo_marcar(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota);


#endif