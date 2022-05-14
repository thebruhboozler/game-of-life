
CC = gcc

Libs = -lGL -lglfw -lglut 

cFiles = src/main.c src/renderSys.c src/controls.c src/menu.c src/commands.c src/chunkSys.c src/game.c



main.o: $(cFiles)
	$(CC) $(cFiles) $(Libs) -g -o GameOfLife.out