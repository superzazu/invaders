BIN = invaders
CC = cc
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
	rm -f $(BIN) *.o
