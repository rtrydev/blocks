#include "world.h"
#include "cube.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include "types.h"
#include "player.h"
#include "frustum.h"

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

WorldState worldState = {
    .chunks = NULL,
    .chunkCount = 36
};

void getChunksInProximity(int proximity, Chunk* chunks) {
    Vector3 playerPosition = getPlayerState().position;

    int currentChunk = 0;

    for (int i = 0; i < worldState.chunkCount; i++) {
        if (worldState.chunks[i].position.x > playerPosition.x - (double)CHUNK_SIZE
            && worldState.chunks[i].position.x < playerPosition.x + (double)CHUNK_SIZE
            && worldState.chunks[i].position.y > playerPosition.y - (double)CHUNK_SIZE
            && worldState.chunks[i].position.y < playerPosition.y + (double)CHUNK_SIZE
            && worldState.chunks[i].position.z > playerPosition.z - (double)CHUNK_SIZE
            && worldState.chunks[i].position.z < playerPosition.z + (double)CHUNK_SIZE
        ) {
            chunks[currentChunk++] = worldState.chunks[i];
        }
    }
}

void generateWorld() {
    srand(123);
    worldState.chunks = calloc(worldState.chunkCount, sizeof(Chunk));

    for (int j = 0; j < worldState.chunkCount; j++) {
        double offset = (double)CHUNK_SIZE * (double)(worldState.chunkCount / (int)sqrt(worldState.chunkCount)) / 2.0;

        Vector3 chunkPosition = {
        .x = 0.0 + (double)CHUNK_SIZE * (double)(j / (int)sqrt(worldState.chunkCount)) - offset,
        .y = 0.0,
        .z = 0.0 + (double)CHUNK_SIZE * (double)(j % (int)sqrt(worldState.chunkCount)) - offset
        };
        worldState.chunks[j].position = chunkPosition;
        worldState.chunks[j].gameElements = calloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, sizeof(GameElement));

        for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            int x = i % CHUNK_SIZE + chunkPosition.x;
            int y = (i / CHUNK_SIZE) % CHUNK_SIZE + chunkPosition.y;
            int z = i / (CHUNK_SIZE * CHUNK_SIZE) + chunkPosition.z;

            Vector3 elementPosition = {
                .x = (double)x,
                .y = (double)y,
                .z = (double)z
            };
            worldState.chunks[j].gameElements[i].position = elementPosition;

            if (y == 0) {
                worldState.chunks[j].gameElements[i].elementType = 1;
            }

            if (x > 5 && y > 0 && z > 5) {
                if ((double)rand() / RAND_MAX < 0.05) {
                    worldState.chunks[j].gameElements[i].elementType = 1;
                }
            }
        }
    }
}


void removeWorld() {
    for (int i = 0; i < worldState.chunkCount; i++) {
        free(worldState.chunks[i].gameElements);
    }
    free(worldState.chunks);
}

void drawWorld(const Frustum* frustum) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    for (int j = 0; j < worldState.chunkCount; j++) {
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            if (worldState.chunks[j].gameElements[i].elementType == 1) {
                Vector3 basePosition = worldState.chunks[j].gameElements[i].position;
                Vector3 cubeCenter;
                Vector3 cubeExtents;
                
                getCubeAABB(basePosition, &cubeCenter, &cubeExtents);
                
                if (isAABBInFrustum(frustum, &cubeCenter, &cubeExtents)) {
                    drawCube(basePosition);
                }
            }
        }
    }

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void getGameElementsInProximity(Vector3 position, GameElement** gameElements) {
    (*gameElements) = calloc(36, sizeof(GameElement));

    int chunksInProximityCount = 8;
    Chunk* chunksInProximity = calloc(chunksInProximityCount, sizeof(Chunk));

    getChunksInProximity(1, chunksInProximity);

    int currentGameElement = 0;

    for (int j = 0; j < chunksInProximityCount; j++) {
        if (chunksInProximity[j].gameElements == NULL) {
            continue;
        }

        for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            int x = i % CHUNK_SIZE;
            int y = (i / CHUNK_SIZE) % CHUNK_SIZE;
            int z = i / (CHUNK_SIZE * CHUNK_SIZE);

            Vector3 currentElementPosition = chunksInProximity[j].gameElements[i].position;

            bool isNearX = currentElementPosition.x < position.x + 1.5 && currentElementPosition.x > position.x - 1.5;
            bool isNearY = currentElementPosition.y < position.y + 3.0 && currentElementPosition.y > position.y - 1.0;
            bool isNearZ = currentElementPosition.z < position.z + 1.5 && currentElementPosition.z > position.z - 1.5;

            if (isNearX && isNearY && isNearZ) {
                (*gameElements)[currentGameElement++] = chunksInProximity[j].gameElements[i];
            }
        }
    }

    free(chunksInProximity);
}
