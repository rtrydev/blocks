#include "world.h"
#include "cube.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include "types.h"
#include "player.h"
#include "frustum.h"
#include <stdio.h> // Added for printf

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

// Updated signature to include depthMapFBO
void drawWorld(const Frustum* frustum, const DepthMapFBO* depthMapFBO) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Get current projection and modelview matrices for HZB check
    GLfloat projectionMatrix[16];
    GLfloat modelViewMatrix[16];
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);

    for (int j = 0; j < worldState.chunkCount; j++) {
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            if (worldState.chunks[j].gameElements[i].elementType == 1) {
                Vector3 basePosition = worldState.chunks[j].gameElements[i].position;
                Vector3 cubeCenter;
                Vector3 cubeExtents;
                
                getCubeAABB(basePosition, &cubeCenter, &cubeExtents);
                
                if (isAABBInFrustum(frustum, &cubeCenter, &cubeExtents)) {
                    // Calculate AABB min/max in world space for HZB check
                    Vector3 aabbMinWorld = {
                        cubeCenter.x - cubeExtents.x,
                        cubeCenter.y - cubeExtents.y,
                        cubeCenter.z - cubeExtents.z
                    };
                    Vector3 aabbMaxWorld = {
                        cubeCenter.x + cubeExtents.x,
                        cubeCenter.y + cubeExtents.y,
                        cubeCenter.z + cubeExtents.z
                    };

                    // Perform HZB occlusion check
                    if (isOccludedByHZB(&aabbMinWorld, &aabbMaxWorld, depthMapFBO, projectionMatrix, modelViewMatrix)) {
                        // Log that the cube was occluded
                        printf("Cube at (%.1f, %.1f, %.1f) with center (%.1f, %.1f, %.1f) occluded by HZB.\n", 
                               basePosition.x, basePosition.y, basePosition.z,
                               cubeCenter.x, cubeCenter.y, cubeCenter.z);
                    } else {
                        drawCube(basePosition);
                    }
                }
            }
        }
    }

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawWorldDepth(const Frustum* frustum) {
    // Note: This function assumes the depth shader program is already active
    // and appropriate uniforms (model, view, projection) are set.
    // It also assumes that cube VBOs are initialized.

    glEnableClientState(GL_VERTEX_ARRAY); // Still need to enable vertex array for glVertexPointer

    for (int j = 0; j < worldState.chunkCount; j++) {
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            if (worldState.chunks[j].gameElements[i].elementType == 1) {
                Vector3 basePosition = worldState.chunks[j].gameElements[i].position;
                Vector3 cubeCenter;
                Vector3 cubeExtents;
                
                // AABB check for culling is still useful for the depth pass
                getCubeAABB(basePosition, &cubeCenter, &cubeExtents);
                
                if (isAABBInFrustum(frustum, &cubeCenter, &cubeExtents)) {
                    drawCubeDepth(basePosition); // Call the simplified depth drawing function
                }
            }
        }
    }

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
