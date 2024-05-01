#include "world.h"
#include "cube.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

void generateWorld(int**** worldStructure) {
    (*worldStructure) = calloc(16, sizeof(int**));
    for (size_t i = 0; i < 16; i++) {
        (*worldStructure)[i] = calloc(16, sizeof(int*));
        for (size_t j = 0; j < 16; j++) {
            (*worldStructure)[i][j] = calloc(16, sizeof(int));
        }
    }

    for (size_t i = 0; i < 16; i++) {
        for (size_t j = 0; j < 16; j++) {
            (*worldStructure)[i][0][j] = 1;
        }
    }

    (*worldStructure)[3][1][3] = 1;
    (*worldStructure)[3][1][4] = 1;
    (*worldStructure)[4][1][3] = 1;
    (*worldStructure)[4][1][4] = 1;
    (*worldStructure)[5][1][3] = 1;
    (*worldStructure)[5][1][4] = 1;

    (*worldStructure)[4][2][3] = 1;
    (*worldStructure)[4][2][4] = 1;
    (*worldStructure)[5][2][3] = 1;
    (*worldStructure)[5][2][4] = 1;

    (*worldStructure)[5][3][3] = 1;
    (*worldStructure)[5][3][4] = 1;

    (*worldStructure)[4][7][3] = 1;
    (*worldStructure)[4][7][4] = 1;
    (*worldStructure)[5][7][3] = 1;
    (*worldStructure)[5][7][4] = 1;
}


void removeWorld(int**** worldStructure) {
    for (size_t i = 0; i < 16; i++) {
        for (size_t j = 0; j < 16; j++) {
            free((*worldStructure)[i][j]);
        }
        free((*worldStructure)[i]);
    }

    free(*worldStructure);
}

void drawWorld(int**** worldStructure) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 16; k++) {
                if ((*worldStructure)[i][j][k] == 1) {
                    drawCube((double)i, (double)j, (double)k);
                }
            }
        }
    }
}

void searchForCollision(
    double positionX, double positionY, double positionZ,
    double directionX, double directionY, double directionZ,
    bool* isCollisionX, bool* isCollisionY, bool* isCollisionZ,
    int**** worldStructure
) {
    for (double i = -0.25; i < 0.49; i += 0.25) {
        int quantizedPositionX = (int)round(positionX + i);
        int quantizedPositionY = (int)round(positionY);
        int quantizedPositionZ = (int)round(positionZ + i);

        int approachingX = (int)round(positionX - i + directionX);
        int approachingY = (int)round(positionY + 0.49 + directionY);
        int approachingZ = (int)round(positionZ - i + directionZ);

        // Out of bounds
        if (quantizedPositionX < 0 || quantizedPositionX > 15 || approachingX < 0) {
            return;
        }

        if (quantizedPositionY < 0 || quantizedPositionY > 15 || approachingY > 13) {
            return;
        }

        if (quantizedPositionZ < 0 || quantizedPositionZ > 15 || approachingZ < 0) {
            return;
        }

        // Process actual collisions
        if ((*worldStructure)[approachingX][quantizedPositionY + 1][quantizedPositionZ] == 1) {
            *isCollisionX = true;
        }

        if ((*worldStructure)[quantizedPositionX][approachingY + 1][quantizedPositionZ] == 1) {
            *isCollisionY = true;
        }

        if ((*worldStructure)[quantizedPositionX][approachingY + 2][quantizedPositionZ] == 1) {
            *isCollisionY = true;
        }

        if ((*worldStructure)[quantizedPositionX][quantizedPositionY + 1][approachingZ] == 1) {
            *isCollisionZ = true;
        }

        if ((*worldStructure)[approachingX][quantizedPositionY + 1][approachingZ] == 1) {
            *isCollisionX = true;
            *isCollisionZ = true;
        }
    }
}
