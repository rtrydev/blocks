#include "cube.h"
#include "types.h"

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#endif

// Global VBO IDs
static GLuint vboVertexId = 0;
static GLuint vboColorId = 0;
static GLuint vboOutlineColorId = 0;

const GLfloat vertices[] =
{
    -0.5, -0.5, -0.5,   -0.5, -0.5,  0.5,   -0.5,  0.5,  0.5,   -0.5,  0.5, -0.5,
     0.5, -0.5, -0.5,    0.5, -0.5,  0.5,    0.5,  0.5,  0.5,    0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,   -0.5, -0.5,  0.5,    0.5, -0.5,  0.5,    0.5, -0.5, -0.5,
    -0.5,  0.5, -0.5,   -0.5,  0.5,  0.5,    0.5,  0.5,  0.5,    0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,   -0.5,  0.5, -0.5,    0.5,  0.5, -0.5,    0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,   -0.5,  0.5,  0.5,    0.5,  0.5,  0.5,    0.5, -0.5,  0.5
};

const GLfloat colors[] =
{
    0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
    0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
    0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
    0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
    0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
    0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0
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

void drawCube(Vector3 position) {
    glPushMatrix();
    glTranslatef(position.x + 0.5, position.y - 1.0, position.z + 0.5);

    // Bind vertex VBO
    glBindBuffer(GL_ARRAY_BUFFER, vboVertexId);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    // Bind color VBO for filled cube
    glBindBuffer(GL_ARRAY_BUFFER, vboColorId);
    glColorPointer(3, GL_FLOAT, 0, NULL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_QUADS, 0, 24);

    // Bind outline color VBO for outline
    glBindBuffer(GL_ARRAY_BUFFER, vboOutlineColorId);
    glColorPointer(3, GL_FLOAT, 0, NULL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_QUADS, 0, 24);

    glPopMatrix();

    // Unbind VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initCubeVBOs() {
    // Generate buffer IDs
    glGenBuffers(1, &vboVertexId);
    glGenBuffers(1, &vboColorId);
    glGenBuffers(1, &vboOutlineColorId);

    // Load vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vboVertexId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Load color data
    glBindBuffer(GL_ARRAY_BUFFER, vboColorId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // Load outline color data
    glBindBuffer(GL_ARRAY_BUFFER, vboOutlineColorId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(outlineColors), outlineColors, GL_STATIC_DRAW);

    // Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void freeCubeVBOs() {
    glDeleteBuffers(1, &vboVertexId);
    glDeleteBuffers(1, &vboColorId);
    glDeleteBuffers(1, &vboOutlineColorId);
}
