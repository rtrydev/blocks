#include "cube.h"
#include "types.h"

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#endif

static GLuint vboVertexId = 0;
static GLuint vboColorId = 0;
// static GLuint vboOutlineColorId = 0; // Removed as per requirement

const GLfloat vertices[] =
{
    -0.5, -0.5, -0.5,   -0.5, -0.5,  0.5,   -0.5,  0.5,  0.5,   -0.5,  0.5, -0.5,
     0.5, -0.5, -0.5,    0.5, -0.5,  0.5,    0.5,  0.5,  0.5,    0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,   -0.5, -0.5,  0.5,    0.5, -0.5,  0.5,    0.5, -0.5, -0.5,
    -0.5,  0.5, -0.5,   -0.5,  0.5,  0.5,    0.5,  0.5,  0.5,    0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,   -0.5,  0.5, -0.5,    0.5,  0.5, -0.5,    0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,   -0.5,  0.5,  0.5,    0.5,  0.5,  0.5,    0.5, -0.5,  0.5
};

const GLfloat outlineColors[] =
{
    1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
    1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
    1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
    1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
    1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
    1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1
};

GLint getColorByType(int type) {
    switch (type) {
    case 0:
        return 0x000000;
    case 1:
        return 0x224422;
    case 2:
        return 0xE5D8A6;
    case 3:
        return 0x6CA0DC;
    default:
        return 0x000000;
    }
}

static void hexToRGB(GLuint hex, GLfloat* rgb) {
    rgb[0] = ((hex >> 16) & 0xFF) / 255.0f; // Red
    rgb[1] = ((hex >> 8) & 0xFF) / 255.0f;  // Green
    rgb[2] = (hex & 0xFF) / 255.0f;         // Blue
}

void drawCube(Vector3 position, GLuint hexColor, GLuint outlineHexColor) {
    GLfloat faceColors[24 * 3];
    GLfloat rgb[3];
    hexToRGB(hexColor, rgb);

    // Fill all 24 vertices with the same color
    for (int i = 0; i < 24; ++i) {
        faceColors[i * 3 + 0] = rgb[0];
        faceColors[i * 3 + 1] = rgb[1];
        faceColors[i * 3 + 2] = rgb[2];
    }

    glPushMatrix();
    glTranslatef(position.x + 0.5f, position.y - 1.0f, position.z + 0.5f);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertexId);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    // Use client-side color array for block color
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glColorPointer(3, GL_FLOAT, 0, faceColors);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_QUADS, 0, 24);

    // Disable color array for flat outline color
    glDisableClientState(GL_COLOR_ARRAY); 

    // Convert outlineHexColor to RGB
    GLfloat rgbOutline[3];
    hexToRGB(outlineHexColor, rgbOutline);

    // Set outline color
    glColor3f(rgbOutline[0], rgbOutline[1], rgbOutline[2]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_QUADS, 0, 24);

    // Re-enable color array for subsequent cube faces
    glEnableClientState(GL_COLOR_ARRAY); 

    glPopMatrix();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initCubeVBOs() {
    glGenBuffers(1, &vboVertexId);
    // glGenBuffers(1, &vboOutlineColorId); // Removed as per requirement

    glBindBuffer(GL_ARRAY_BUFFER, vboVertexId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glBindBuffer(GL_ARRAY_BUFFER, vboOutlineColorId); // Removed as per requirement
    // glBufferData(GL_ARRAY_BUFFER, sizeof(outlineColors), outlineColors, GL_STATIC_DRAW); // Removed as per requirement

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void freeCubeVBOs() {
    glDeleteBuffers(1, &vboVertexId);
    glDeleteBuffers(1, &vboColorId);
    // glDeleteBuffers(1, &vboOutlineColorId); // Removed as per requirement
}
