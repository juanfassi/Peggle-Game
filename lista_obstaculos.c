#include <stdlib.h>
#include <assert.h>
#include <SDL2/SDL.h>


#include "lista_obstaculos.h"
#include "poligono.h"
#include "config.h"

/*
Lee informacion del archivo 
*/
bool leer_encabezado(FILE *f, color_t *color, movimiento_t *movimiento, geometria_t *geometria){
    uint8_t encabezado; 
    if(fread(&encabezado, sizeof(uint8_t), 1, f)!=1)
        return false;
    
    *color=(encabezado & MASK_COLOR) >> SHIFT_COLOR;
    *movimiento=(encabezado & MASK_MOV) >> SHIFT_MOV;
    *geometria=encabezado & MASK_GEOM;

    return true;
}

/*
Pide memoria para obstaculo_t y guarda la informacion obtenida en leer_encabezado, 
por lo que se devuelve un obstaculo_t con informacion. 
Si falla retorna NULL
*/
obstaculo_t*obstaculo_crear_desde_archivo(FILE*f){
    geometria_t geometria;
    obstaculo_t *obstaculo;

    if((obstaculo=malloc(sizeof(obstaculo_t)))==NULL)
        return NULL;
    leer_encabezado(f, &obstaculo->color, &obstaculo->movimiento, &geometria);

    size_t n_parametros;
    int16_t parametros[3];
    assert(leer_movimiento(f, obstaculo->movimiento, parametros, &n_parametros));
    for(size_t i=0; i<n_parametros;i++)
        obstaculo->parametros[i]=(float)parametros[i];        

    obstaculo->forma=leer_geometria(f, geometria); 
    
    return obstaculo;   
}

/*
libera obstaculo_t y sus componentes
*/
void obstaculo_destruir(obstaculo_t *obs){
    poligono_destruir(obs->forma);
    free(obs);
}

/*Lee la informacion del archivo, guardandola en el obstaculo_t que se crea al llamar a la funcion obstaculo_crear_desde_archivo.
Devuelve un puntero doble a obstaculo_t con un arreglo con todos los obstaculos del nivel*/
obstaculo_t** obstaculo_leer_archivo(FILE*f,size_t *n_obstaculos){ 
    obstaculo_t **selector;
    
    fread(n_obstaculos, sizeof(int16_t), 1, f);

    selector=malloc((*n_obstaculos)*sizeof(obstaculo_t*));
    if(selector==NULL) 
        return NULL;
    for(size_t obstaculo = 0; obstaculo < (*n_obstaculos); obstaculo++){
        if((selector[obstaculo]=obstaculo_crear_desde_archivo(f))==NULL){
            for(size_t i=0; i<obstaculo;i++)
                obstaculo_destruir(*(selector+i));
            free(selector);
            return NULL;
        } 
    }
    return selector;

}

/*
Crea el nodo de la lista. Pide memoria para esta.
*/
static struct nodo *nodo_crear(struct nodo *sig, obstaculo_t *dato) {
    struct nodo *n = malloc(sizeof(struct nodo));
    if(n == NULL) return NULL;

    n->sig = sig;
    n->dato = dato;

    return n;
}

/*
Crea la lista desde el archivo.
*/
lista_t * lista_crear_desde_archivo(FILE *f, size_t*obs){
    lista_t *l=lista_crear();
    obstaculo_t **selector=obstaculo_leer_archivo(f,obs);
    if(selector==NULL)
        free(l);
    for(size_t i=0; i<*obs;i++)
        lista_insertar_al_final(l, selector[i]);
    free(selector);
    return l;
}

/*
Pide memoria para lista_t
*/
lista_t *lista_crear() {
    lista_t *l = malloc(sizeof(lista_t));
    if(l == NULL)
        return NULL;

    l->prim = NULL;

    return l;
}

/*
Libera la memoria pedida para lista_t como asi tambien su dato (obstaculo_t*)
*/
void lista_destruir(lista_t *l) {
    struct nodo *actual = l->prim;
    while(actual != NULL) {
        struct nodo *siguiente = actual;

       obstaculo_destruir(actual->dato);
        actual = actual->sig;
        free(siguiente);
    }

    free(l);
}

/*
Crea el nuevo nodo e inserta al final de la lista el dato (obstaculo_t*)
*/
bool lista_insertar_al_final(lista_t *l, obstaculo_t *dato) {
    struct nodo *nuevo = nodo_crear(NULL, dato);
    if(nuevo == NULL) return false;

    if(l->prim == NULL) {
        l->prim = nuevo;
        return true;
    }

    struct nodo *actual = l->prim;
    while(actual->sig != NULL) {
        actual = actual->sig;
    }

    actual->sig = nuevo;

    return true;
}

/*
Indica el largo de la lista. No se utiliza en este momento del programa. 
Se utilizo al principio para obtener informacion sobre la cantidad de obstaculos.
*/
int lista_largo(struct nodo *l){
    struct nodo *actual = l->sig;

    if(actual==NULL)
    return 0;

    return lista_largo(actual) + 1;
}

/*
Puntero a funcion que evidencia el movimiento que presenta cada uno de los obstaculo_t
*/
typedef bool (*mover_t)(obstaculo_t*);

mover_t opciones_movimiento[] = {
    [MOV_INMOVIL] = generar_movimiento_inmovil,
    [MOV_CIRCULAR] = generar_movimiento_circular,
    [MOV_HORIZONTAL] = generar_movimiento_horizontal,
};

bool generar_movimiento_inmovil(obstaculo_t* m){
    return true;
}

bool generar_movimiento_circular(obstaculo_t* m){
    rotar_respecto_centro(m->forma,m->parametros[2]*DT,m->parametros[0], m->parametros[1]);
    return true;
}

bool generar_movimiento_horizontal(obstaculo_t* m){
    float dx =(m->parametros[2])*DT;

    if((dx>0) && (m->parametros[1] > (m->parametros[0]-dx)))
        m->parametros[2]*=-1;
    
    if((dx<0) && (m->parametros[1] < -dx))
        m->parametros[2]*=-1;
   
    for(size_t i=0;i<m->forma->n;i++)
        m->forma->vertices[i][0]=m->forma->vertices[i][0]+dx;
    m->parametros[1]+=dx;
    return true;
}

bool mover(obstaculo_t* m){
    return opciones_movimiento[m->movimiento](m);
}

/*
Llama a la funcion mover y genera el movimiento del obstaculo segun lo indicado en su estructura
*/
void lista_mover(const lista_t *l) {
    struct nodo *actual = l->prim;
    while(actual != NULL) {
        mover(actual->dato); 
        actual = actual->sig;
    }
}

/*
Borra un color determinado que se pasa por argumento. Modifica la cantidad de obstaculos que quedan en el nivel.
*/
void lista_borrar_elemento(lista_t *l, color_t eliminar, size_t * obstaculos) {  
    if(l->prim == NULL) 
        return;
    struct nodo *anterior = NULL;
    struct nodo *actual = l->prim;

    while(actual != NULL) {
        if(actual->dato->color==eliminar){
            struct nodo *aux=actual;
            if(anterior!=NULL)
                anterior->sig=actual->sig;
            else{
                l->prim=l->prim->sig;
            }
            actual=actual->sig;
            obstaculo_destruir(aux->dato);
            free(aux);
            (*obstaculos)--;
        }
        else{
            anterior=actual;
            actual=actual->sig;
        }    
    }
}

typedef poligono_t* (*lectura_geometria_t)(FILE *f);

lectura_geometria_t arr_lectura_geometria[] = {
    [GEO_CIRCULO] = leer_geometria_circulo,
    [GEO_RECTANGULO] = leer_geometria_rectangulo,
    [GEO_POLIGONO] = leer_geometria_poligono,
};

poligono_t *leer_geometria_circulo(FILE *f){
    int16_t parametros[PARAMETRO_GEO_CIRCULO];

    if((fread(parametros, sizeof(int16_t), PARAMETRO_GEO_CIRCULO, f))!=PARAMETRO_GEO_CIRCULO)
        return NULL;

    return generar_bola(parametros[2],parametros[0],parametros[1]);
}

poligono_t *leer_geometria_rectangulo(FILE *f){

    int16_t parametros[PARAMETRO_GEO_RECTANGULO];

    if((fread(parametros, sizeof(int16_t), PARAMETRO_GEO_RECTANGULO, f))!=PARAMETRO_GEO_RECTANGULO)
        return NULL;

    float vertices[][2]={
    {parametros[0]-(parametros[2]/2.0),parametros[1]-(parametros[3]/2.0)},
    {parametros[0]+(parametros[2]/2.0),parametros[1]-(parametros[3]/2.0)},
    {parametros[0]+(parametros[2]/2.0),parametros[1]+(parametros[3]/2.0)},
    {parametros[0]-(parametros[2]/2.0),parametros[1]+(parametros[3]/2.0)}
    };

    float alpha_rad=(parametros[4]*PI)/180;

    poligono_t *rectangulo=poligono_crear(vertices, 4);
    rotar_respecto_centro(rectangulo,alpha_rad,parametros[0],parametros[1]);

    return rectangulo;

}

poligono_t *leer_geometria_poligono(FILE *f){
    int16_t cantidad_puntos;

    if((fread(&cantidad_puntos, sizeof(int16_t), 1, f))!=1)
        return NULL;

    if(cantidad_puntos==0)
        return NULL;

    int16_t coordenadas[cantidad_puntos][2];
    float coordenada[cantidad_puntos][2];


    if((fread(coordenadas, sizeof(int16_t), 2*cantidad_puntos, f))!=2*cantidad_puntos) //no se si esta bien...
        return NULL;

    for(size_t i=0;i<cantidad_puntos;i++){
        coordenada[i][0]=(float)coordenadas[i][0];
        coordenada[i][1]=(float)coordenadas[i][1];
    }

    poligono_t *poligono=poligono_crear(coordenada,cantidad_puntos);

    return poligono;
}

poligono_t *leer_geometria(FILE*f, geometria_t geometria){
    return arr_lectura_geometria[geometria](f);
}

/*
Funciones: rotar, trasladar y rotar_respecto_centro  se usan para el movimiento de los osbtaculos
*/

void rotar(float poligono[][2], size_t n, double rad){
    int filas;
    float x,y;
    float cos_angulo=cos(rad);
    float sen_angulo=sin(rad);
    for (filas=0; filas<n; filas++){
        x=poligono[filas][0];
        y=poligono[filas][1];
        poligono[filas][0]=(x*cos_angulo-y*sen_angulo);
        poligono[filas][1]=(x*sen_angulo+y*cos_angulo);        
    }
}

void trasladar (float poligono[][2], size_t n, float dx, float dy){
    int filas;
    for (filas=0; filas<n; filas++){
        poligono[filas][0]+=dx;
        poligono[filas][1]+=dy;
    }
}

void rotar_respecto_centro(poligono_t *poligono,float angulo,float centro_x, float centro_y){
    trasladar (poligono->vertices,poligono->n,-centro_x,-centro_y);
    rotar(poligono->vertices, poligono->n,angulo);
    trasladar (poligono->vertices,poligono->n,centro_x,centro_y);       
}

/*
Lee la informacion (parametros) que se encuentra en el archivo, segun el tipo de movimiento que ejecuta cada obstaculo.
*/
typedef bool (*manejador2_t)(FILE *f, int16_t parametros[], size_t *n_parametros);

manejador2_t manejadores[] = {
    [MOV_INMOVIL] = leer_movimiento_inmovil,
    [MOV_CIRCULAR] = leer_movimiento_circular,
    [MOV_HORIZONTAL] = leer_movimiento_horizontal,
};

bool leer_movimiento_inmovil(FILE *f, int16_t parametros[], size_t *n_parametros){
    *n_parametros=0;
    return true;
}

bool leer_movimiento_circular(FILE *f, int16_t parametros[], size_t *n_parametros){
    *n_parametros=3;

    if((fread(parametros,sizeof(int16_t),*n_parametros,f))!=*n_parametros)
        return false;

    return true;
}

bool leer_movimiento_horizontal(FILE *f, int16_t parametros[], size_t *n_parametros){
    *n_parametros=3;

    if((fread(parametros,sizeof(int16_t),*n_parametros,f))!=*n_parametros)
        return false;

    return true;
}

bool leer_movimiento(FILE *f, movimiento_t movimiento, int16_t parametros[],size_t *n_parametros){
    return manejadores[movimiento](f,parametros,n_parametros);
}
