#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "poligono.h"
#include "config.h"

/*
Pide memoria para poligono_t. Si falla retorna NULL
*/
poligono_t *poligono_crear(float vertices[][2], size_t n){
    poligono_t *poligono = malloc(sizeof(poligono_t)); 
    if (poligono==NULL)
        return NULL;

    poligono->vertices = malloc(2*n*sizeof(float));
    if(poligono->vertices == NULL) {
        free(poligono);
        return NULL;
    }

    for(size_t fil=0;fil<n;fil++){
        poligono->vertices[fil][0]=vertices[fil][0];
        poligono->vertices[fil][1]=vertices[fil][1];
    }

    poligono->n=n;

    return poligono;
}

/*
Dependiendo del radio y la resolucion requerida de la bola a crear determina los puntos necesarios.
Con estos llama a la funcion poligono_crear
*/
poligono_t *generar_bola(float radio, float centro_x, float centro_y){
    size_t cantidad_puntos=((2*PI)/RES_BOLA); 
    float arr[cantidad_puntos][2];
    size_t pos=0;

    for (float i=0;pos<cantidad_puntos;i+=RES_BOLA, pos++){  
        arr[pos][0]=radio*cos(i)+centro_x;
        arr[pos][1]=radio*sin(i)+centro_y;
    }

    return (poligono_crear(arr,cantidad_puntos));
}

/*
Libera la memoria pedida para generar un tipo poligono_t
*/
void poligono_destruir(poligono_t *p){
    free(p->vertices);
    free(p);
}

