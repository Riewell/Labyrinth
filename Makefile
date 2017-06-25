CC=gcc
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

all: labyrinth.o labyrinth_generation.o sdl_main.o parameters.o
	$(CC) -o labyrinth labyrinth.o labyrinth_generation.o sdl_main.o parameters.o help.o $(SDL_LDFLAGS) -lSDL2_image 

labyrinth.o: labyrinth.c
	$(CC) -c -Wall -g -std=c99 -o labyrinth.o labyrinth.c

labyrinth_generation.o: labyrinth_generation.c
	$(CC) -c -Wall -g -std=c99 -o labyrinth_generation.o labyrinth_generation.c

sdl_main.o: sdl_main.c
	$(CC) $(SDL_CFLAGS) -c -Wall -g -std=c99 -o sdl_main.o sdl_main.c

parameters.o: parameters.c help.o
	$(CC) -c -Wall -g -std=c99 -o parameters.o parameters.c

help.o: help.c
	$(CC) -c -Wall -g -std=c99 -o help.o help.c

clean:
	rm -rf *.o
