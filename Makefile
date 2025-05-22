detected_OS := $(shell uname)

CC = gcc

CFLAGS = -pthread -Wall -O3

ifeq ($(detected_OS),Darwin)
	INCLUDE := -I$(shell brew --prefix)/include
	LIBS := -L$(shell brew --prefix)/lib -lglfw -framework OpenGL
endif

ifeq ($(detected_OS),Linux)
	LIBS := -lglfw -lGLU -lGL -lGLEW -lglut -lm
endif

SRCS = src/main.c src/engine/cube.c src/engine/window.c src/engine/display.c src/engine/player.c src/engine/world.c src/engine/userinputs.c src/engine/viewport.c src/engine/gametime.c src/engine/forces.c src/engine/frustum.c

OUTPUT = blocks

all: $(OUTPUT)

$(OUTPUT): $(SRCS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRCS) $(INCLUDE) $(LIBS)

clean:
	rm -f $(OUTPUT)
