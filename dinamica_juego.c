#include <math.h>
#include <stdio.h>

#include "dinamica_juego.h"
#include "graficador.h"
#include "lista_obstaculos.h"
#include "config.h"

/*
Funcion que decide si la pelota no se ha movido de un lugar reducido por un tiempo dado.
Verdadero si esta en esa zona. Falso si no lo esta
*/
bool es_trabada(size_t cont_tiempo, float cx, float cy, float cx_ref, float cy_ref){
    if(cont_tiempo%TIEMPO_ESTIMADO==0)
        if((cy<=cy_ref+0.1 && cy>=cy_ref-0.1) && (cx<=cx_ref+0.1 && cx>=cx_ref-0.1))
            return true;
    return false;
}
/*
Funcion que determina la cantidad de obstaculos naranjas en el nivel.
Funcion que se utiliza para saber si paso el nivel
*/
size_t cant_naranja(lista_t *lista){
    size_t naranja=0;
    struct nodo*actual=lista->prim;
    while (actual!=NULL){
        if(actual->dato->color==NARANJA)
            naranja++;
        actual=actual->sig;
    }
    return naranja;
}
/*
Funcion que cuenta la cantidad de obstaculos naranjas, si es igual a 0 entonces gana el nivel y retorna verdadero.
*/
bool gana_nivel(lista_t *l){
    if(cant_naranja(l)==0){
        return true;
    }
    return false;
}
/*
Puntero a funciones  que determina la suma de cada obstaculo marcado.
Si es verde aumenta a su vez una bola disponible para disparar.
Si es naranja, dependiendo de la cantidad de naranjas ya marcadas se aumenta el score de diferente forma
*/
typedef void(*suma_t)(obstaculo_t *, size_t *, size_t *, int *);

suma_t suma[] = {
    [AZUL] = suma_azul,
    [NARANJA] = suma_naranja,
    [VERDE] = suma_verde,
};

void suma_azul(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota){
    if(*naranja<=10)
        (*score) +=10; 
    else {
        if(*naranja<=15)
            (*score)+=20;
        else{
            if(*naranja<=19)
                (*score)+=30; 
            else{  
                if(*naranja<=21)
                    (*score)+=50;
                else
                    (*score)+=100;   
            }
        }
    }        
}

void suma_verde(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota){
    (*contador_pelota) ++;

    if(*naranja<=10)
        (*score) +=10; 
    else {
        if(*naranja<=15)
            (*score)+=20;
        else{
            if(*naranja<=19)
                (*score)+=30; 
            else{  
                if(*naranja<=21)
                    (*score)+=50;
                else
                    (*score)+=100;   
            }
        }
    }    
}

void suma_naranja(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota){
    (*naranja)++;

    if(*naranja<=10)
        (*score) +=100; 
    else {
        if(*naranja<=15)
            (*score)+=200;
        else{
            if(*naranja<=19)
                (*score)+=300; 
            else{  
                if(*naranja<=21)
                    (*score)+=500;
                else
                    (*score)+=1000;   
            }
        }
    }    
}

void obstaculo_sumar(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota){
    suma[obs->color](obs, score, naranja, contador_pelota);
}

/*
Si no termino el nivel y perdio todas las bolas disponibles se reinicia el nivel 
con la cantidad de bolas del inicio de la partida
*/
void reiniciar_nivel(int *contador_pelotas,int * nivel,int bits_movidos,FILE*f){
    fseek(f,bits_movidos,SEEK_SET);
    *contador_pelotas=CANT_PELOTAS_INICIO;
    (*nivel)--;
}

/*
Marca un obstaculo si este es de color azul, verde o naranja, retornando Verdadero. Sino retorna Falso.
*/
bool obstaculo_marcar(obstaculo_t *obs, size_t *score, size_t *naranja, int * contador_pelota){
    if(obs->color==GRIS || obs->color==BLANCO)
        return false;
    
    obstaculo_sumar(obs, score, naranja, contador_pelota);

    obs->color=BLANCO;
    return true;
}