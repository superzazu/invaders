UNAME := $(shell uname)
TARGET = invaders
CC = clang
CFLAGS = -std=c11 -g -Wall
ifeq ($(UNAME), Linux)
LIBS = -lglfw -lGL
endif
ifeq ($(UNAME), Darwin)
LIBS = -lglfw -framework OpenGL
endif

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
