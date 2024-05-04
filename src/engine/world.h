#ifndef BLOCKS_WORLD
#define BLOCKS_WORLD

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
} WorldState;

void generateWorld();
void removeWorld();
void drawWorld();
void getGameElementsInProximity(Vector3 position, GameElement** gameElements);

#endif
