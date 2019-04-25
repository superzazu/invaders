bin = invaders
src = $(wildcard *.c) 8080/i8080.c
obj = $(src:.c=.o)

CFLAGS = -g -Wall -Wextra -O2 -std=c99 -pedantic -Wno-gnu-binary-literal $(shell pkg-config --cflags sdl2 SDL2_mixer)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_mixer)

ifeq ($(MAKECMDGOALS),web)
CC = emcc
CFLAGS = -g -Wall -Wextra -O2 -std=c99 -pedantic -s USE_SDL=2 -s USE_SDL_MIXER=2
LDFLAGS = -s USE_SDL=2 -s USE_SDL_MIXER=2 --preload-file res
endif

.PHONY: all clean

all: $(bin)

$(bin): $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

web: $(obj)
	$(CC) $^ $(LDFLAGS) -o invaders.html

clean:
	-rm $(bin) $(obj)
	-rm invaders.js invaders.wasm invaders.data invaders.html
