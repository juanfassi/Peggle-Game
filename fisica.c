#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include "fisica.h"
#include "poligono.h"
#include "lista_obstaculos.h"
#include "config.h"
#include "dinamica_juego.h"

/*
Devuelve la velocidad final
*/
double computar_velocidad(double vi, double a, double dt){
    return a*dt + vi; 
}

/*
Devuelve la posicion final
*/
double computar_posicion(double pi, double vi, double dt){
    return vi*dt + pi;
}

/*
Producto interno de un vector R2.
*/
float producto_interno(float ax, float ay, float bx, float by){
    return ((ax*bx)+(ay*by));
}

/*
Obtiene la distancia de dos puntos
*/
double distancia(float xa, float ya, float xb, float yb){
    return sqrt(((xa-xb)*(xa-xb))+((ya-yb)*(ya-yb)));
}

void punto_mas_cercano(float x0, float y0, float x1, float y1, float xp, float yp, float *x, float *y) {
    float ax = xp - x0;
    float ay = yp - y0;
    float bx = x1 - x0;
    float by = y1 - y0;

    float alfa = producto_interno(ax, ay, bx, by) / producto_interno(bx, by, bx, by);

    if(alfa <= 0) {
        *x = x0;
        *y = y0;
    }
    else if(alfa >= 1) {
        *x = x1;
        *y = y1;
    }
    else {
        *x = alfa * bx + x0;
        *y = alfa * by + y0;
    }
}

void reflejar(float norm_x, float norm_y, float *cx, float *cy, float *vx, float *vy) {
    float proy = producto_interno(norm_x, norm_y, *vx, *vy);

    if(proy >= 0)
        return;
    *vx -= 2 * norm_x * proy * PLASTICIDAD;
    *vy -= 2 * norm_y * proy * PLASTICIDAD;

    *cx += norm_x * 0.1;
    *cy += norm_y * 0.1;
}

double poligono_distancia(const poligono_t *p, float xp, float yp, float *nor_x, float *nor_y) {
    double d = 1 / 0.0;
    size_t idx = 0;

    for(size_t i = 0; i < p->n; i++) {
        float xi, yi;
        punto_mas_cercano(p->vertices[i][0], p->vertices[i][1], p->vertices[(i + 1) % p->n][0], p->vertices[(i + 1) % p->n][1], xp, yp, &xi, &yi);
        double di = distancia(xp, yp, xi, yi);

        if(di < d) {
            d = di;
            idx = i;
        }
    }

    float nx = p->vertices[(idx + 1) % p->n][1] - p->vertices[idx][1];
    float ny = p->vertices[idx][0] - p->vertices[(idx + 1) % p->n][0];
    float dn = distancia(nx, ny, 0, 0);

    nx /= dn;
    ny /= dn;

    *nor_x = nx;
    *nor_y = ny;

    return d;
}

/*
Recorre la lista de obstaculos, si la distancia a la bola es menor a su 
radio entonces devuelve true y llama a la funcion reflejar y marca 
el obstaculo con el cual impacto
*/
bool buscar_rebote(float *cx, float *cy,float *vx,float *vy,lista_t *lista, size_t *score, size_t *naranja, int *contador_pelota, bool* contacto){ 
    struct nodo *actual = lista->prim;
    float nor_x;
    float nor_y; 
    while (actual!=NULL){
        if(poligono_distancia(actual->dato->forma,*cx, *cy, &nor_x,&nor_y)<=2*BOLA_RADIO){
            reflejar(nor_x,nor_y, cx,cy,vx, vy);
            if(obstaculo_marcar(actual->dato, score, naranja, contador_pelota))
                *contacto=true;
            return true;
        }
        actual=actual->sig;
    }
    return false;
}

/*
Con la posicion y el radio de la bola se guardan los puntos de los vertices de la bola en el tipo poligono_t
*/
void calcular_pos_bola(poligono_t *bola, float radio, float centro_x, float centro_y){
    size_t pos=0;
    for (float i=0;pos<bola->n;i+=RES_BOLA, pos++){  
        bola->vertices[pos][0]=radio*cos(i)+centro_x;
        bola->vertices[pos][1]=radio*sin(i)+centro_y;
        }
}