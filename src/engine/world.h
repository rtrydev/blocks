#ifndef BLOCKS_WORLD
#define BLOCKS_WORLD
#define CHUNK_SIZE 16

#include <stdbool.h>
#include "types.h"
#include "frustum.h"

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
// Modified signature to include DepthMapFBO for HZB culling
void drawWorld(const Frustum* frustum, const DepthMapFBO* depthMapFBO); 
void drawWorldDepth(const Frustum* frustum); // New function for depth pass
void getGameElementsInProximity(Vector3 position, GameElement** gameElements);

#endif
