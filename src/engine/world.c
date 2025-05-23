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

#include "constants.h"

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

WorldState* getWorldStateGlobal() {
    return &worldState;
}

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

static float valueNoise2d(float x, float z) {
    float total = 0.0f;
    float frequency = 0.08f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    int octaves = 4;

    for (int i = 0; i < octaves; i++) {
        float n = sinf(x * frequency + i * 13.37f) * cosf(z * frequency + i * 7.13f);
        total += n * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    return total / maxValue;
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

            float height = valueNoise2d((float)x, (float)z) * 10.0f;
            int groundHeight = (int)roundf(height);

            if (y <= 0) {
                worldState.chunks[j].gameElements[i].elementType = 3;
            }
            else if (y == groundHeight && groundHeight <= 1) {
                worldState.chunks[j].gameElements[i].elementType = 2;
            }
            else if (y <= groundHeight) {
                worldState.chunks[j].gameElements[i].elementType = 1;
            }
            else {
                worldState.chunks[j].gameElements[i].elementType = 0;
            }
        }
    }

    for (int j = 0; j < worldState.chunkCount; j++) {
        for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            worldState.chunks[j].gameElements[i].isObstructed = false;
        }
    }

    for (int j = 0; j < worldState.chunkCount; j++) {
        for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            GameElement* currentBlock = &worldState.chunks[j].gameElements[i];

            if (currentBlock->elementType == 0) {
                continue;
            }

            int solidNeighborCount = 0;

            int currentX = (int)floor(currentBlock->position.x);
            int currentY = (int)floor(currentBlock->position.y);
            int currentZ = (int)floor(currentBlock->position.z);

            if (getBlockAtGlobal(&worldState, currentX + 1, currentY, currentZ) != NULL) {
                solidNeighborCount++;
            }
            if (getBlockAtGlobal(&worldState, currentX - 1, currentY, currentZ) != NULL) {
                solidNeighborCount++;
            }
            if (getBlockAtGlobal(&worldState, currentX, currentY + 1, currentZ) != NULL) {
                solidNeighborCount++;
            }
            if (getBlockAtGlobal(&worldState, currentX, currentY - 1, currentZ) != NULL) {
                solidNeighborCount++;
            }
            if (getBlockAtGlobal(&worldState, currentX, currentY, currentZ + 1) != NULL) {
                solidNeighborCount++;
            }
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
    PlayerState pState = getPlayerState(); // Get current player state

    for (int j = 0; j < worldState.chunkCount; j++) {
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            GameElement gameElement = worldState.chunks[j].gameElements[i];

            if (gameElement.elementType != 0 && !gameElement.isObstructed) {
                Vector3 basePosition = gameElement.position;
                Vector3 cubeCenter;
                Vector3 cubeExtents;
                
                getCubeAABB(basePosition, &cubeCenter, &cubeExtents);
                
                if (isAABBInFrustum(frustum, &cubeCenter, &cubeExtents)) {
                    bool isHighlighted = false;
                    if (pState.isLookingAtBlock && pState.lookingAtBlock != NULL) {
                        if (pState.lookingAtBlock->x == gameElement.position.x &&
                            pState.lookingAtBlock->y == gameElement.position.y &&
                            pState.lookingAtBlock->z == gameElement.position.z) {
                            isHighlighted = true;
                        }
                    }

                    if (isHighlighted) {
                        glLineWidth(2.0f); // Set line width for highlighted block
                        drawCube(basePosition, getColorByType(gameElement.elementType), HIGHLIGHT_OUTLINE_COLOR);
                        glLineWidth(1.0f); // Reset line width to default
                    } else {
                        // Ensure non-highlighted blocks are drawn with default line width
                        // (though glLineWidth(1.0f) after highlighted block should cover this,
                        // it's good practice if there were other paths)
                        drawCube(basePosition, getColorByType(gameElement.elementType), DEFAULT_OUTLINE_COLOR);
                    }
                }
            }
        }
    }
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

        int chunk_origin_x = (int)floor(chunk->position.x);
        int chunk_origin_y = (int)floor(chunk->position.y);
        int chunk_origin_z = (int)floor(chunk->position.z);

        if (x >= chunk_origin_x && x < chunk_origin_x + CHUNK_SIZE &&
            y >= chunk_origin_y && y < chunk_origin_y + CHUNK_SIZE &&
            z >= chunk_origin_z && z < chunk_origin_z + CHUNK_SIZE) {
            
            int local_x = x - chunk_origin_x;
            int local_y = y - chunk_origin_y;
            int local_z = z - chunk_origin_z;

            int index = local_x + local_y * CHUNK_SIZE + local_z * CHUNK_SIZE * CHUNK_SIZE;

            GameElement* element = &chunk->gameElements[index];

            if (element->elementType != 0) {
                return element;
            } else {
                return NULL; 
            }
        }
    }
    return NULL;
}
