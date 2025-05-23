#include "world.h"
#include "cube.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
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

void getChunksInProximity(Vector3 position, int proximity, Chunk* chunks) {
    int currentChunk = 0;

    for (int i = 0; i < worldState.chunkCount; i++) {
        if (worldState.chunks[i].position.x > position.x - (double)CHUNK_SIZE
            && worldState.chunks[i].position.x < position.x + (double)CHUNK_SIZE
            && worldState.chunks[i].position.y > position.y - (double)CHUNK_SIZE
            && worldState.chunks[i].position.y < position.y + (double)CHUNK_SIZE
            && worldState.chunks[i].position.z > position.z - (double)CHUNK_SIZE
            && worldState.chunks[i].position.z < position.z + (double)CHUNK_SIZE
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
                if ((double)rand() / RAND_MAX < 1.5) {
                    worldState.chunks[j].gameElements[i].elementType = 1;
                }
            }
        }
    }

    // Initialize isObstructed flags
    for (int j = 0; j < worldState.chunkCount; j++) {
        for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            worldState.chunks[j].gameElements[i].isObstructed = false;
        }
    }

    // New occlusion logic
    for (int j = 0; j < worldState.chunkCount; j++) {
        for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            GameElement* currentBlock = &worldState.chunks[j].gameElements[i];

            if (currentBlock->elementType == 0) { // Skip air blocks
                continue;
            }

            int solidNeighborCount = 0;

            // Get integer position of currentBlock
            // Note: Block positions are already integers effectively after generation.
            // Using floor here ensures conversion from the stored double, though direct casting might also work
            // if positions are guaranteed to be whole numbers. Floor is safer.
            int currentX = (int)floor(currentBlock->position.x);
            int currentY = (int)floor(currentBlock->position.y);
            int currentZ = (int)floor(currentBlock->position.z);

            // Check +X neighbor
            if (getBlockAtGlobal(&worldState, currentX + 1, currentY, currentZ) != NULL) {
                solidNeighborCount++;
            }
            // Check -X neighbor
            if (getBlockAtGlobal(&worldState, currentX - 1, currentY, currentZ) != NULL) {
                solidNeighborCount++;
            }
            // Check +Y neighbor
            if (getBlockAtGlobal(&worldState, currentX, currentY + 1, currentZ) != NULL) {
                solidNeighborCount++;
            }
            // Check -Y neighbor
            if (getBlockAtGlobal(&worldState, currentX, currentY - 1, currentZ) != NULL) {
                solidNeighborCount++;
            }
            // Check +Z neighbor
            if (getBlockAtGlobal(&worldState, currentX, currentY, currentZ + 1) != NULL) {
                solidNeighborCount++;
            }
            // Check -Z neighbor
            if (getBlockAtGlobal(&worldState, currentX, currentY, currentZ - 1) != NULL) {
                solidNeighborCount++;
            }

            if (solidNeighborCount == 6) {
                currentBlock->isObstructed = true;
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
            GameElement gameElement = worldState.chunks[j].gameElements[i];

            if (gameElement.elementType != 0 && !gameElement.isObstructed) {
                Vector3 basePosition = gameElement.position;
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

void getGameElementsInProximity(Vector3 position, Vector3 rangeFrom, Vector3 rangeTo, GameElement** gameElements) {
    (*gameElements) = calloc(36, sizeof(GameElement));

    int chunksInProximityCount = 8;
    Chunk* chunksInProximity = calloc(chunksInProximityCount, sizeof(Chunk));

    getChunksInProximity(position, 1, chunksInProximity);

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

            bool isNearX = currentElementPosition.x < position.x + rangeTo.x && currentElementPosition.x > position.x - rangeFrom.x;
            bool isNearY = currentElementPosition.y < position.y + rangeTo.y && currentElementPosition.y > position.y - rangeFrom.y;
            bool isNearZ = currentElementPosition.z < position.z + rangeTo.z && currentElementPosition.z > position.z - rangeFrom.z;

            if (isNearX && isNearY && isNearZ) {
                (*gameElements)[currentGameElement++] = chunksInProximity[j].gameElements[i];
            }
        }
    }

    free(chunksInProximity);
}

GameElement* getBlockAtGlobal(WorldState* worldState, int x, int y, int z) {
    for (int i = 0; i < worldState->chunkCount; i++) {
        Chunk* chunk = &worldState->chunks[i];

        // Determine chunk boundaries using integer coordinates
        int chunk_origin_x = (int)floor(chunk->position.x);
        int chunk_origin_y = (int)floor(chunk->position.y);
        int chunk_origin_z = (int)floor(chunk->position.z);

        // Check if the global coordinates are within this chunk's boundaries
        if (x >= chunk_origin_x && x < chunk_origin_x + CHUNK_SIZE &&
            y >= chunk_origin_y && y < chunk_origin_y + CHUNK_SIZE &&
            z >= chunk_origin_z && z < chunk_origin_z + CHUNK_SIZE) {
            
            // Calculate local coordinates
            int local_x = x - chunk_origin_x;
            int local_y = y - chunk_origin_y;
            int local_z = z - chunk_origin_z;

            // Calculate the index into the gameElements array
            // No need to validate local_x, local_y, local_z here as the previous check
            // (global coordinates within chunk boundaries) and floor usage for origin
            // should ensure they are within [0, CHUNK_SIZE - 1].
            int index = local_x + local_y * CHUNK_SIZE + local_z * CHUNK_SIZE * CHUNK_SIZE;

            // Also, no need to explicitly check if index is < CHUNK_SIZE^3, as valid local coordinates
            // will always produce a valid index.

            GameElement* element = &chunk->gameElements[index];

            if (element->elementType != 0) {
                return element; // Found a non-air block
            } else {
                // Block is air, but coordinates are valid.
                // Per instruction "If the coordinates do not fall within any chunk, OR if the block at those coordinates is air ... return NULL"
                // So, if we find air, we should return NULL immediately as this is the correct block for the given x,y,z.
                return NULL; 
            }
        }
    }
    return NULL; // No chunk contains these coordinates, or the block found was air.
}
