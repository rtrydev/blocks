#ifndef BLOCKS_WORLD
#define BLOCKS_WORLD
#define CHUNK_SIZE 16

#include <stdbool.h>
#include "types.h"
#include "frustum.h"

typedef struct GameElement {
	Vector3 position;
	int elementType;
	bool isObstructed;
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
void drawWorld(const Frustum* frustum);
void getGameElementsInProximity(Vector3 position, Vector3 rangeFrom, Vector3 rangeTo, GameElement** gameElements);

#endif
