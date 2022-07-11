#include <SDL2/SDL.h>

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "fisica.h"
#include "poligono.h"
#include "graficador.h"
#include "dinamica_juego.h"
#include "lista_obstaculos.h"

#ifdef TTF
#include <SDL2/SDL_ttf.h>


void escribir_texto(SDL_Renderer *renderer, TTF_Font *font, const char *s, int x, int y) {
    SDL_Color color = {255, 255, 255};  // Estaría bueno si la función recibe un enumerativo con el color, ¿no?
    SDL_Surface *surface = TTF_RenderText_Solid(font, s, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
#endif

int main(int argc, char *argv[]){
    size_t score=0; 
    size_t naranja=0; //bolas naranjas tocadas
    int contador_pelota=CANT_PELOTAS_INICIO; //cantidad de pelotas disponibles
    size_t cont_tiempo=0; //para evaluar posteriormente si la pelota quedo trabada
    int bits_movidos; //para volver a leer el nivel del archivo si no lo gano
    bool contacto=false; 
    SDL_Init(SDL_INIT_VIDEO);

    if(argc != 2) {
        fprintf(stderr, "Uso: %s <archivo>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if(f == NULL) {
        fprintf(stderr, "No pudo abrirse \"%s\"\n", argv[1]);
        return 1;
    }


    #ifdef TTF
        TTF_Init();
        TTF_Font* font = TTF_OpenFont("FreeSansBold.ttf", 24);
    #endif

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_CreateWindowAndRenderer(VENTANA_ANCHO, VENTANA_ALTO, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Peggle");

    int dormir = 0;

    // BEGIN código del alumno
    float canon_angulo = 0; // Ángulo del cañón
    bool cayendo = false;   // ¿Hay bola disparada?

    float cx, cy;   // Centro de la bola
    float vx, vy;   // Velocidad de la bola
    float cx_ref, cy_ref;

    //Generamos las pelotas disponibles para graficar
    poligono_t *pelotas=generar_bola(PELOTAS_RADIO,PELOTAS_RADIO+BOLA_RADIO,PELOTAS_RADIO); 
    if(pelotas==NULL)
        return 1;

    //Generamos la bola a disparar
    poligono_t*bola=generar_bola(BOLA_RADIO,cx,cy);
    if(bola==NULL){
        poligono_destruir(pelotas);
        fclose(f);
        #ifdef TTF
            TTF_CloseFont(font);
            TTF_Quit();
        #endif  
        return 1;
    }

    //Generamos las pelotas naranjas tocadas para graficar
    poligono_t*p_naranjas=generar_bola(RADIO_NARANJA,MAX_X + RADIO_NARANJA ,MAX_Y);
    if(p_naranjas==NULL){
        poligono_destruir(pelotas);
        poligono_destruir(bola);
        fclose(f);
        #ifdef TTF
            TTF_CloseFont(font);
            TTF_Quit();
        #endif  
        return 1;
    }

    // END código del alumno

    unsigned int ticks = SDL_GetTicks();

    int nivel = 0;
    while(1){
        bits_movidos=ftell(f);

        if(nivel==ULTIMO_NIVEL+1)
            break; 

        nivel++;

        //para imprimir el numero de nivel
        char aux[10]; 
        sprintf(aux,"Nivel %d",nivel); 

        size_t obstaculos=0;
        lista_t* lista=lista_crear_desde_archivo(f, &obstaculos);
        if(lista==NULL){
            poligono_destruir(bola);
            poligono_destruir(pelotas);
            poligono_destruir(p_naranjas);
            fclose(f);
            #ifdef TTF
                TTF_CloseFont(font);
                TTF_Quit();
            #endif            
            return 1;
        }
        //fin crear lista
        while(1) {
            if(SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    break;

                if (event.type == SDL_KEYDOWN) //cambia de nivel con una tecla
                    break;

                // BEGIN código del alumno
                if(event.type == SDL_MOUSEBUTTONDOWN) {
                    if(! cayendo)
                        cayendo = true;
                }
                else 
                    if (event.type == SDL_MOUSEMOTION) {
                        canon_angulo = atan2(event.motion.x - CANON_X, event.motion.y - CANON_Y);
                        if(canon_angulo > CANON_MAX)
                            canon_angulo = CANON_MAX;
                        if(canon_angulo < -CANON_MAX)
                            canon_angulo = -CANON_MAX;
                    }
                
                // END código del alumno
                continue;
            }
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);


            // BEGIN código del alumno
            char puntaje[20];
            sprintf(puntaje, "Score: %zd", score);
            char bolas_disponibles[20];
            sprintf(bolas_disponibles,"Bolas: %d", contador_pelota);
            char naranjas_tocada[20];
            sprintf(naranjas_tocada,"%zd", naranja);
            if(nivel!=7){
            #ifdef TTF
                    escribir_texto(renderer, font, "Peggle", 120, 20);
                    escribir_texto(renderer, font, puntaje, 330, 20);
                    escribir_texto(renderer, font, aux, 600, 20);
                    escribir_texto(renderer, font, bolas_disponibles, 0, 10);
                    escribir_texto(renderer, font, naranjas_tocada, MAX_X+35, MAX_Y-15);
            #endif
            }
            else{
                #ifdef TTF
                    escribir_texto(renderer, font, "Peggle", 330, 20);
                    escribir_texto(renderer, font, puntaje, 330, MAX_Y/2);
                    escribir_texto(renderer, font, "Ganaste!!!!", 330, MAX_Y/2 - 30);
                    escribir_texto(renderer, font, bolas_disponibles, 0, 10);
                    escribir_texto(renderer, font, naranjas_tocada, MAX_X+35, MAX_Y-15);
                #endif
            }

            cy_ref=cy;
            cx_ref=cx;

            if(cayendo) {
                // Si la bola está cayendo actualizamos su posición
                buscar_rebote(&cx,&cy,&vx,&vy,lista,&score,&naranja, &contador_pelota, &contacto);
    
                vy = computar_velocidad(vy, G, DT);
                vx *= ROZAMIENTO;
                vy *= ROZAMIENTO;
                cx = computar_posicion(cx, vx, DT);
                cy = computar_posicion(cy, vy, DT);                
            }
            else {
                // Si la bola no se disparó establecemos condiciones iniciales
                cx = CANON_X + CANON_LARGO * sin(canon_angulo);
                cy = CANON_Y + CANON_LARGO * cos(canon_angulo);
                vx = BOLA_VI * sin(canon_angulo);
                vy = BOLA_VI * cos(canon_angulo);
            }

            // Rebote contra las paredes:
            if(cx < MIN_X + BOLA_RADIO || cx > MAX_X - BOLA_RADIO) 
                vx = - vx;
            if(cy < MIN_Y + BOLA_RADIO) 
                vy = -vy;

            // Se salió de la pantalla: elimina los elementos blancos y si no hay mas elementos pasa de nivel
            if(cy > MAX_Y - BOLA_RADIO){
                lista_borrar_elemento(lista, BLANCO, &obstaculos); 
                cayendo = false;
                if(!contacto)
                    contador_pelota++;
                contacto=false;
                if(gana_nivel(lista))
                    break;
                contador_pelota--;
            }
            cont_tiempo++;
            if(es_trabada(cont_tiempo, cx,cy,cx_ref,cy_ref)==true)
                lista_borrar_elemento(lista, BLANCO, &obstaculos); 
                
            // Dibujamos el cañón:
            SDL_RenderDrawLine(renderer, CANON_X, CANON_Y, CANON_X + sin(canon_angulo) * CANON_LARGO, CANON_Y + cos(canon_angulo) * CANON_LARGO);

            //Dibujamos la pelota que se dispara
            calcular_pos_bola(bola,BOLA_RADIO,cx,cy);
            dibujar_poligono(bola,renderer,BLANCO);

            
            //dibujamos las pelotas que quedan disponibles
            for(size_t i=0; i<contador_pelota;i++){
                dibujar_pelotas(pelotas,contador_pelota ,PELOTAS_RADIO, renderer);
                calcular_pos_bola(pelotas,PELOTAS_RADIO,PELOTAS_RADIO+BOLA_RADIO,3*PELOTAS_RADIO+2*PELOTAS_RADIO*i);
            }

            //dibuja hasta 60 pelotas naranjas golpeadas
            naranjas_golpeadas(p_naranjas, naranja, RADIO_NARANJA-1, renderer);

            // Dibujamos las paredes:
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00);
            SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MAX_X, MIN_Y);
            SDL_RenderDrawLine(renderer, MIN_X, MAX_Y, MAX_X, MAX_Y);
            SDL_RenderDrawLine(renderer, MIN_X, MAX_Y, MIN_X, MIN_Y);
            SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MAX_X, MIN_Y);

            // configura la lista, color y movimiento
            configurar_lista(lista, renderer);            

            // Dibujamos el vector de velocidad:
            SDL_SetRenderDrawColor(renderer, 193, 255, 255, 0xFF);
            SDL_RenderDrawLine(renderer, cx, cy, cx + vx, cy + vy);

            // END código del alumno

            SDL_RenderPresent(renderer);
            ticks = SDL_GetTicks() - ticks;
            if(dormir) {
                SDL_Delay(dormir);
                dormir = 0;
            }
            else if(ticks < 1000 / JUEGO_FPS)
                SDL_Delay(1000 / JUEGO_FPS - ticks);
            ticks = SDL_GetTicks();

            if(contador_pelota==0){
                reiniciar_nivel(&contador_pelota,&nivel,bits_movidos,f);
                break;
            } 

        }        
        //cuando termina el nivel libero todo
        lista_destruir(lista);

        if (event.type == SDL_QUIT)
            break;
    }

    // BEGIN código del alumno
    poligono_destruir(bola);
    poligono_destruir(pelotas);
    poligono_destruir(p_naranjas);

    // END código del alumno

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    #ifdef TTF
        TTF_CloseFont(font);
        TTF_Quit();
    #endif

    SDL_Quit();
    fclose(f);
    return 0;
}

