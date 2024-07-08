#ifndef BLOCKS_WORLD
#define BLOCKS_WORLD
#define CHUNK_SIZE 16

#include <stdbool.h>
#include "types.h"

typedef struct GameElement {
	Vector3 position;
	int elementType;
} GameElement;

typedef struct Chunk {
	Vector3 position;
	GameElement* gameElements;
} Chunk;

typedef struct WorldState {
	Chunk* chunks;
	int chunkCount;
} WorldState;

void generateWorld();
void removeWorld();
void drawWorld();
void getGameElementsInProximity(Vector3 position, GameElement** gameElements);

#endif
