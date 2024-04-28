CC = gcc

CFLAGS = -pthread -Wall -O3

LIBS = -lglfw -lGLU -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl -lm

SRCS = src/main.c src/cube.c src/window.c src/display.c

OUTPUT = blocks

all: $(OUTPUT)

$(OUTPUT): $(SRCS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRCS) $(LIBS)

clean:
	rm -f $(OUTPUT)
