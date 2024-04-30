ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname)
endif

CC = gcc

CFLAGS = -pthread -Wall -O3

ifeq ($(detected_OS),Darwin)
	INCLUDE := -I$(shell brew --prefix)/include
	LIBS := -L$(shell brew --prefix)/lib -lglfw -framework OpenGL
endif

ifeq ($(detected_OS),Linux)
	LIBS := -lglfw -lGLU -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl -lm
endif

SRCS = src/main.c src/cube.c src/window.c src/display.c src/player.c

OUTPUT = blocks

all: $(OUTPUT)

$(OUTPUT): $(SRCS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRCS) $(INCLUDE) $(LIBS)

clean:
	rm -f $(OUTPUT)
