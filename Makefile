detected_OS := $(shell uname)

CC = gcc

CFLAGS = -pthread -Wall -O3

ifeq ($(detected_OS),Darwin)
	INCLUDE := -I$(shell brew --prefix)/include
	LIBS := -L$(shell brew --prefix)/lib -lglfw -framework OpenGL
endif

ifeq ($(detected_OS),Linux)
	LIBS := -lglfw -lGLU -lGL -lm
endif

SRCS = src/main.c src/cube.c src/window.c src/display.c src/player.c src/world.c

OUTPUT = blocks

all: $(OUTPUT)

$(OUTPUT): $(SRCS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRCS) $(INCLUDE) $(LIBS)

clean:
	rm -f $(OUTPUT)
