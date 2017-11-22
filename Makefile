UNAME := $(shell uname)
TARGET = invaders
CC = cc
CFLAGS = -g -Wall -O3
LIBS = `pkg-config --cflags --libs sdl2 SDL2_mixer`

# add OpenGL
ifeq ($(UNAME), Linux)
LIBS += -lGL
endif
ifeq ($(UNAME), Darwin)
LIBS += -framework OpenGL
endif

.PHONY: default all clean

default: $(TARGET)
all: default

SOURCES = main.c invaders.c 8080/i8080.c audio.c
HEADERS = invaders.h 8080/i8080.h 8080/types.h audio.h

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(SOURCES)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
