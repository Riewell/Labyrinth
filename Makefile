CC=gcc
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

all: labyrinth_main.o labyrinth_generation.o checking_in_neighbor_cells.o sdl_main.o sdl_main_hwsw.o parameters.o
	$(CC) -o labyrinth labyrinth_main.o labyrinth_generation.o checking_in_neighbor_cells.o sdl_main.o sdl_main_hwsw.o parameters.o help.o $(SDL_LDFLAGS) -lSDL2_image

labyrinth_main.o: labyrinth_main.c
	$(CC) -c -Wall -g -std=c99 -o labyrinth_main.o labyrinth_main.c

labyrinth_generation.o: labyrinth_generation.c
	$(CC) -c -Wall -g -std=c99 -o labyrinth_generation.o labyrinth_generation.c

checking_in_neighbor_cells.o: checking_in_neighbor_cells.c
	$(CC) $(SDL_CFLAGS) -c -Wall -g -std=c99 -o checking_in_neighbor_cells.o checking_in_neighbor_cells.c

sdl_main.o: sdl_main.c
	$(CC) $(SDL_CFLAGS) -c -Wall -g -std=c99 -o sdl_main.o sdl_main.c

sdl_main_hwsw.o: sdl_main_hwsw.c
	$(CC) $(SDL_CFLAGS) -c -Wall -g -std=c99 -o sdl_main_hwsw.o sdl_main_hwsw.c

parameters.o: parameters.c help.o
	$(CC) -c -Wall -g -std=c99 -o parameters.o parameters.c

help.o: help.c
	$(CC) -c -Wall -g -std=c99 -o help.o help.c

clean:
	rm -rf *.o

