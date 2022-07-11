#ifndef _OBSTACULO_H_
#define _OBSTACULO_H_

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "poligono.h" 
#include "lista_obstaculos.h"


void dibujar_obstaculo(obstaculo_t *obs,SDL_Renderer *renderer);

void rgb_colores(color_t color, uint8_t*r,uint8_t*g,uint8_t*b);

void rgb_azul(uint8_t*r,uint8_t*g,uint8_t*b); 

void rgb_naranja(uint8_t*r,uint8_t*g,uint8_t*b);

void rgb_verde(uint8_t*r,uint8_t*g,uint8_t*b);

void rgb_gris(uint8_t*r,uint8_t*g,uint8_t*b);

void rgb_blanco(uint8_t*r,uint8_t*g,uint8_t*b);

void rgb_purpura(uint8_t*r,uint8_t*g,uint8_t*b);

void dibujar(void*,void*);

void lista_dibujar(const lista_t *l, SDL_Renderer *renderer);

void configurar_lista(const lista_t *l, SDL_Renderer *renderer);

void dibujar_pelotas(poligono_t *pelotas, int cant_pelotas, float radio, SDL_Renderer *renderer);

void dibujar_poligono(poligono_t *poligono,SDL_Renderer *renderer, color_t color);

void naranjas_golpeadas(poligono_t* p, size_t naranjas, float dx, SDL_Renderer *renderer);


#endif