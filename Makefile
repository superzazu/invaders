BIN = invaders
CC = cc
ifeq ($(MAKECMDGOALS),web)
CC = emcc
endif
CFLAGS = -g -Wall -Wextra -O3 -std=c11 $(shell pkg-config --cflags sdl2)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_mixer)

SOURCES = main.c invaders.c 8080/i8080.c audio.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: clean

default: $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJECTS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) *.o 8080/*.o
	rm -f invaders.js invaders.wasm invaders.data invaders.html

web: $(OBJECTS)
	$(CC) $(OBJECTS) -s USE_SDL=2 --preload-file res -o invaders.html
