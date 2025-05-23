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
#include <stdio.h>

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

void placeBlock(WorldState* ws, int x, int y, int z, int blockType) {
    if (getBlockAtGlobal(ws, x, y, z) != NULL) {
        return;
    }

    Chunk* targetChunk = NULL;
    int local_x = 0, local_y = 0, local_z = 0;
    int elementIndex = 0;

    for (int i = 0; i < ws->chunkCount; i++) {
        Chunk* chunk = &ws->chunks[i];
        int chunk_origin_x = (int)floor(chunk->position.x);
        int chunk_origin_y = (int)floor(chunk->position.y);
        int chunk_origin_z = (int)floor(chunk->position.z);

        if (x >= chunk_origin_x && x < chunk_origin_x + CHUNK_SIZE &&
            y >= chunk_origin_y && y < chunk_origin_y + CHUNK_SIZE &&
            z >= chunk_origin_z && z < chunk_origin_z + CHUNK_SIZE) {
            
            targetChunk = chunk;
            local_x = x - chunk_origin_x;
            local_y = y - chunk_origin_y;
            local_z = z - chunk_origin_z;
            elementIndex = local_x + local_y * CHUNK_SIZE + local_z * CHUNK_SIZE * CHUNK_SIZE;
            break;
        }
    }

    if (targetChunk == NULL) {
        return;
    }

    GameElement* newBlock = &targetChunk->gameElements[elementIndex];
    newBlock->elementType = blockType;
    newBlock->isObstructed = false;
    newBlock->position.x = (double)x;
    newBlock->position.y = (double)y;
    newBlock->position.z = (double)z;

    int dx[] = {-1, 1, 0, 0, 0, 0};
    int dy[] = {0, 0, -1, 1, 0, 0};
    int dz[] = {0, 0, 0, 0, -1, 1};

    int solidNeighborCountNewBlock = 0;
    for (int i = 0; i < 6; i++) {
        GameElement* neighbor = getBlockAtGlobal(ws, x + dx[i], y + dy[i], z + dz[i]);
        if (neighbor != NULL && neighbor->elementType != 0) {
            solidNeighborCountNewBlock++;
        }
    }
    newBlock->isObstructed = (solidNeighborCountNewBlock == 6);

    for (int i = 0; i < 6; i++) {
        int neighborX = x + dx[i];
        int neighborY = y + dy[i];
        int neighborZ = z + dz[i];

        GameElement* neighborBlock = getBlockAtGlobal(ws, neighborX, neighborY, neighborZ);

        if (neighborBlock != NULL && neighborBlock->elementType != 0) {
            int solidNeighborCount = 0;
            for (int j = 0; j < 6; j++) {
                GameElement* nnBlock = getBlockAtGlobal(ws, neighborX + dx[j], neighborY + dy[j], neighborZ + dz[j]);
                if (nnBlock != NULL && nnBlock->elementType != 0) {
                    solidNeighborCount++;
                }
            }
            neighborBlock->isObstructed = (solidNeighborCount == 6);
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
            int x_coord = i % CHUNK_SIZE + chunkPosition.x;
            int y_coord = (i / CHUNK_SIZE) % CHUNK_SIZE + chunkPosition.y;
            int z_coord = i / (CHUNK_SIZE * CHUNK_SIZE) + chunkPosition.z;

            Vector3 elementPosition = {
                .x = (double)x_coord,
                .y = (double)y_coord,
                .z = (double)z_coord
            };
            worldState.chunks[j].gameElements[i].position = elementPosition;

            float height = valueNoise2d((float)x_coord, (float)z_coord) * 10.0f;
            int groundHeight = (int)roundf(height);

            if (y_coord <= 0) {
                worldState.chunks[j].gameElements[i].elementType = 3;
            }
            else if (y_coord == groundHeight && groundHeight <= 1) {
                worldState.chunks[j].gameElements[i].elementType = 2;
            }
            else if (y_coord <= groundHeight) {
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

            if (getBlockAtGlobal(&worldState, currentX + 1, currentY, currentZ) != NULL) { solidNeighborCount++; }
            if (getBlockAtGlobal(&worldState, currentX - 1, currentY, currentZ) != NULL) { solidNeighborCount++; }
            if (getBlockAtGlobal(&worldState, currentX, currentY + 1, currentZ) != NULL) { solidNeighborCount++; }
            if (getBlockAtGlobal(&worldState, currentX, currentY - 1, currentZ) != NULL) { solidNeighborCount++; }
            if (getBlockAtGlobal(&worldState, currentX, currentY, currentZ + 1) != NULL) { solidNeighborCount++; }
            if (getBlockAtGlobal(&worldState, currentX, currentY, currentZ - 1) != NULL) { solidNeighborCount++; }

            currentBlock->isObstructed = (solidNeighborCount == 6);
        }
    }
}

void removeWorld() {
    if (worldState.chunks != NULL) {
        for (int i = 0; i < worldState.chunkCount; i++) {
            if (worldState.chunks[i].gameElements != NULL) {
                free(worldState.chunks[i].gameElements);
                worldState.chunks[i].gameElements = NULL; 
            }
        }
        free(worldState.chunks);
        worldState.chunks = NULL;
    }
}

void drawWorld(const Frustum* frustum) {
    PlayerState pState = getPlayerState();

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
                        glLineWidth(2.0f);
                        drawCube(basePosition, getColorByType(gameElement.elementType), HIGHLIGHT_OUTLINE_COLOR);
                        glLineWidth(1.0f);
                    } else {
                        drawCube(basePosition, getColorByType(gameElement.elementType), DEFAULT_OUTLINE_COLOR);
                    }
                }
            }
        }
    }
}

void getGameElementsInProximity(Vector3 position, Vector3 rangeFrom, Vector3 rangeTo, GameElement** gameElements) {
    *gameElements = calloc(36, sizeof(GameElement));

    int currentGameElementIndex = 0;
    const int maxGameElements = 36;

    double minX_bound = position.x - rangeFrom.x;
    double maxX_bound = position.x + rangeTo.x;
    double minY_bound = position.y - rangeFrom.y;
    double maxY_bound = position.y + rangeTo.y;
    double minZ_bound = position.z - rangeFrom.z;
    double maxZ_bound = position.z + rangeTo.z;

    int startX = (int)floor(minX_bound) + 1;
    int endX = (int)ceil(maxX_bound) - 1;

    int startY = (int)floor(minY_bound) + 1;
    int endY = (int)ceil(maxY_bound) - 1;

    int startZ = (int)floor(minZ_bound) + 1;
    int endZ = (int)ceil(maxZ_bound) - 1;

    for (int ix = startX; ix <= endX; ++ix) {
        for (int iy = startY; iy <= endY; ++iy) {
            for (int iz = startZ; iz <= endZ; ++iz) {
                if (currentGameElementIndex >= maxGameElements) {
                    goto end_loops;
                }

                GameElement* foundElement = getBlockAtGlobal(&worldState, ix, iy, iz);

                if (foundElement != NULL) {
                    (*gameElements)[currentGameElementIndex++] = *foundElement;
                }
            }
        }
    }

end_loops:;
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

void destroyBlock(WorldState* ws, int x, int y, int z) {
    GameElement* blockToDestroy = getBlockAtGlobal(ws, x, y, z);

    if (blockToDestroy != NULL && blockToDestroy->elementType != 0) {
        blockToDestroy->elementType = 0;

        int dx[] = {-1, 1, 0, 0, 0, 0};
        int dy[] = {0, 0, -1, 1, 0, 0};
        int dz[] = {0, 0, 0, 0, -1, 1};

        for (int i = 0; i < 6; i++) {
            int neighborX = x + dx[i];
            int neighborY = y + dy[i];
            int neighborZ = z + dz[i];

            GameElement* neighborBlock = getBlockAtGlobal(ws, neighborX, neighborY, neighborZ);

            if (neighborBlock != NULL && neighborBlock->elementType != 0) {
                int solidNeighborCount = 0;
                for (int j = 0; j < 6; j++) {
                    GameElement* nnBlock = getBlockAtGlobal(ws, neighborX + dx[j], neighborY + dy[j], neighborZ + dz[j]);
                    if (nnBlock != NULL && nnBlock->elementType != 0) {
                        solidNeighborCount++;
                    }
                }
                neighborBlock->isObstructed = (solidNeighborCount == 6);
            }
        }
    }
}
