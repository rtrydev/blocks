#include "world.h"
#include "cube.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include "types.h"

WorldState worldState = {
    .chunks = NULL
};

void generateWorld() {
    srand(123);

    worldState.chunks = calloc(1, sizeof(Chunk));
    Vector3 chunkPosition = {
        .x = 0.0,
        .y = 0.0,
        .z = 0.0
    };
    worldState.chunks[0].position = chunkPosition;
    worldState.chunks[0].gameElements = calloc(16 * 16 * 16, sizeof(GameElement));

    for (size_t i = 0; i < 16 * 16 * 16; i++) {
        int x = i % 16;
        int y = (i / 16) % 16;
        int z = i / (16 * 16);

        Vector3 elementPosition = {
            .x = (double)x,
            .y = (double)y,
            .z = (double)z
        };
        worldState.chunks[0].gameElements[i].position = elementPosition;

        if (y == 0) {
            worldState.chunks[0].gameElements[i].elementType = 1;
        }

        if (x > 5 && y > 0 && z > 5) {
            if (rand() < 1683) {
                worldState.chunks[0].gameElements[i].elementType = 1;
            }
        }
    }
}


void removeWorld() {
    free(worldState.chunks[0].gameElements);
    free(worldState.chunks);
}

void drawWorld() {
    for (int i = 0; i < 16 * 16 * 16; i++) {
        int x = i % 16;
        int y = (i / 16) % 16;
        int z = i / (16 * 16);

        if (worldState.chunks[0].gameElements[i].elementType == 1) {
            drawCube(worldState.chunks[0].gameElements[i].position);
        }
    }
}

void getGameElementsInProximity(Vector3 position, GameElement** gameElements) {
    (*gameElements) = calloc(36, sizeof(GameElement));

    int currentGameElement = 0;

    for (size_t i = 0; i < 16 * 16 * 16; i++) {
        int x = i % 16;
        int y = (i / 16) % 16;
        int z = i / (16 * 16);

        Vector3 currentElementPosition = worldState.chunks[0].gameElements[i].position;

        bool isNearX = currentElementPosition.x < position.x + 1.5 && currentElementPosition.x > position.x - 1.5;
        bool isNearY = currentElementPosition.y < position.y + 3.0 && currentElementPosition.y > position.y - 1.0;
        bool isNearZ = currentElementPosition.z < position.z + 1.5 && currentElementPosition.z > position.z - 1.5;

        if (isNearX && isNearY && isNearZ) {
            (*gameElements)[currentGameElement++] = worldState.chunks[0].gameElements[i];
        }
    }
}
