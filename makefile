
CC = gcc

Libs = -lGL -lglfw -lglut -lgtk-3 

cFiles = src/main.c src/renderSys.c src/controls.c src/menu.c src/commands.c src/chunkSys.c src/game.c



main.o: $(cFiles)
	$(CC) $(cFiles) $(shell pkg-config --cflags gtk+-3.0) $(Libs)  $(shell pkg-config --libs gtk+-3.0) -g -o GameOfLife.out