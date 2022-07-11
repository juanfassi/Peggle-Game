CBANDERAS= -Wall -Werror -std=c99 -pedantic -lSDL2 -lSDL2_ttf -DTTF -g -lm
CC=gcc

all: peggle

peggle: peggle.o poligono.o graficador.o dinamica_juego.o fisica.o lista_obstaculos.o
	$(CC) -o peggle peggle.o graficador.o poligono.o dinamica_juego.o fisica.o lista_obstaculos.o $(CBANDERAS)

peggle.o: peggle.c config.h poligono.h graficador.h dinamica_juego.h fisica.h lista_obstaculos.h
	$(CC) -o peggle.o -c peggle.c $(CBANDERAS)

poligono.o: poligono.c poligono.h config.h
	$(CC) -o poligono.o -c poligono.c $(CBANDERAS)

graficador.o: graficador.c graficador.h lista_obstaculos.h poligono.h fisica.h
	$(CC) -o graficador.o -c graficador.c $(CBANDERAS)

fisica.o: fisica.c fisica.h poligono.h config.h lista_obstaculos.h dinamica_juego.h
	$(CC) -o fisica.o -c fisica.c $(CBANDERAS)

lista_obstaculos.o: lista_obstaculos.c lista_obstaculos.h poligono.h config.h 
	$(CC) -o lista_obstaculos.o -c lista_obstaculos.c $(CBANDERAS)

dinamica_juego.o: dinamica_juego.c dinamica_juego.h graficador.h lista_obstaculos.h config.h
	$(CC) -o dinamica_juego.o -c dinamica_juego.c $(CBANDERAS)

	
limpiar:
	rm *.o

ejecutar:
	./peggle peggle.bin
	
debug:
	valgrind --suppressions=suppressions_20211_tp1.supp --leak-check=full ./peggle peggle.bin