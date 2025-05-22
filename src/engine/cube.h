#ifndef BLOCKS_CUBE
#define BLOCKS_CUBE

#include "types.h"

void drawCube(Vector3 position);
void drawCubeDepth(Vector3 position); // New function for depth pass

void initCubeVBOs();
void freeCubeVBOs();

#endif