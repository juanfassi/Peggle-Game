#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "graficador.h"
#include "poligono.h"
#include "lista_obstaculos.h"
#include "fisica.h"

/*
Puntero a funciones que setean el color correspondiente por rgb
*/
typedef void(*colores_render_t)(uint8_t*,uint8_t*,uint8_t*);

colores_render_t arr_color_render[]={
    [AZUL]=rgb_azul,
    [NARANJA]=rgb_naranja,
    [VERDE]=rgb_verde,
    [GRIS]=rgb_gris,
    [BLANCO]=rgb_blanco,
    [PURPURA]=rgb_purpura,
};

void rgb_azul(uint8_t*r,uint8_t*g,uint8_t*b){
    *r=0;
    *g=0;
    *b=255;
}

void rgb_naranja(uint8_t*r,uint8_t*g,uint8_t*b){
    *r=255;
    *g=64;
    *b=0;
}

void rgb_verde(uint8_t*r,uint8_t*g,uint8_t*b){
    *r=0;
    *g=193;
    *b=0;
}

void rgb_gris(uint8_t*r,uint8_t*g,uint8_t*b){
    *r=96;
    *g=96;
    *b=96;
}

void rgb_blanco(uint8_t*r,uint8_t*g,uint8_t*b){
    *r=255;
    *g=255;
    *b=255;
}

void rgb_purpura(uint8_t*r,uint8_t*g,uint8_t*b){
    *r=255;
    *g=0;
    *b=128;
}

void rgb_colores(color_t color, uint8_t*r,uint8_t*g,uint8_t*b){
    arr_color_render[color](r,g,b);
}
/*
Recorre la lista y dibuja los obstaculos que se encuentran en ella.
En este TP no se ulitiza, pero se dejo ya que al principio se uso
*/
void lista_dibujar(const lista_t *l, SDL_Renderer *renderer) {
    struct nodo *actual = l->prim;
    while(actual != NULL) {
        dibujar_obstaculo(actual->dato,renderer);
        actual = actual->sig;
    }
}

/*
Recorre la lista y dibuja los obstaculos y le indica el movimiento a realizar cada obstaculo
*/
void configurar_lista(const lista_t *l, SDL_Renderer *renderer) {
    struct nodo *actual = l->prim;
    while(actual != NULL) {
        dibujar_obstaculo(actual->dato,renderer);
        mover(actual->dato);
        actual = actual->sig;
    }
}

#ifdef TTF
/*
dibuja el obstaculo con el color detallado en la estructura obstaculo_t
*/

void dibujar_obstaculo(obstaculo_t *obs,SDL_Renderer *renderer){
    size_t cantidad=obs->forma->n;
    uint8_t r,g,b;

    rgb_colores(obs->color, &r,&g,&b);

    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
    for(size_t i=0;i<cantidad-1;i++)
        SDL_RenderDrawLine(renderer,obs->forma->vertices[i][0],obs->forma->vertices[i][1],obs->forma->vertices[i+1][0],obs->forma->vertices[i+1][1]);
    SDL_RenderDrawLine(renderer,obs->forma->vertices[cantidad-1][0],obs->forma->vertices[cantidad-1][1],obs->forma->vertices[0][0],obs->forma->vertices[0][1]);
}

/*
dDibuja un poligono de estructura poligono_t, donde no se especifica el color, por lo que 
se indica en un argumento de la funcion asi es modificable
*/
void dibujar_poligono(poligono_t *poligono,SDL_Renderer *renderer,color_t color){
    size_t cantidad=poligono->n;
    uint8_t r,g,b;


    rgb_colores(color, &r,&g,&b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);

    for(size_t i=0;i<cantidad-1;i++){
        SDL_RenderDrawLine(renderer,poligono->vertices[i][0],poligono->vertices[i][1],poligono->vertices[i+1][0],poligono->vertices[i+1][1]);        
    }
    SDL_RenderDrawLine(renderer,poligono->vertices[cantidad-1][0],poligono->vertices[cantidad-1][1],poligono->vertices[0][0],poligono->vertices[0][1]);
}

#endif

/*
Dibuja la cantidad de bolas naranjas tocadas, hasta 300 (mas de la cantidad total de bolas naranjas). 
Si se reinicia un nivel y se golpean mas bolas naranjas que el total se continua aumentando el contador, pero no se grafican
*/
void naranjas_golpeadas(poligono_t* p, size_t naranjas, float dx, SDL_Renderer *renderer){
    float dy=1;
    
    for(size_t i=0; i<naranjas && i<100; i++){
        dibujar_poligono(p,renderer, NARANJA);
        dx*=-1;
        calcular_pos_bola(p,RADIO_NARANJA,MAX_X+40+dx,MAX_Y-25-dy);
        dy+=RADIO_NARANJA-1;
    }
    dy=0;
    for(size_t i=100; i<naranjas && i<200; i++){
        dibujar_poligono(p,renderer, NARANJA);
        dx*=-1;
        calcular_pos_bola(p,RADIO_NARANJA,MAX_X+5+RADIO_NARANJA*2+dx,MAX_Y-25-dy-RADIO_NARANJA);
        dy+=(RADIO_NARANJA)-1;
    }
    dy=0;
    for(size_t i=200; i<naranjas && i<300; i++){
        dibujar_poligono(p,renderer, NARANJA);
        dx*=-1;
        calcular_pos_bola(p,RADIO_NARANJA,MAX_X+30+RADIO_NARANJA*6+dx,MAX_Y-25-dy-RADIO_NARANJA);
        dy+=(RADIO_NARANJA)-1;
    }

    if(naranjas>=300)
        return;
}
/*
Dibuja una cantidad de bolas especificadas
*/
void dibujar_pelotas(poligono_t *pelotas, int cant_pelotas, float radio, SDL_Renderer *renderer){
    for(size_t i=0; i<cant_pelotas; i++)
        dibujar_poligono(pelotas,renderer, PURPURA); 
}