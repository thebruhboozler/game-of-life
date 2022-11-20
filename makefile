SOURCE_FILES = src/main.c src/renderSys.c src/controls.c src/menu.c src/commands.c src/chunkSys.c src/game.c src/utils.c
OBJECT_FILES = $(patsubst %.c, %.o, $(SOURCE_FILES))

CFLAGS =  
CFLAGS += $(shell pkg-config --cflags gtk+-3.0 gl glfw3)

LDFLAGS = 
LDFLAGS += $(shell pkg-config --libs gtk+-3.0 gl glfw3) -lglut -lm

.PHONY: all clean

all: GameOfLife.out

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c "$<" -o "$@"

GameOfLife.out: $(OBJECT_FILES)
	$(CC) $(OBJECT_FILES) -o "$@" $(LDFLAGS)

clean:
	rm src/*.o

