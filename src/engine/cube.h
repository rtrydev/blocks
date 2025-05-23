#ifndef BLOCKS_CUBE
#define BLOCKS_CUBE

#include "types.h"
#include <GL/glew.h>

void drawCube(Vector3 position, GLuint hexColor);
GLint getColorByType(int type);

void initCubeVBOs();
void freeCubeVBOs();

#endif