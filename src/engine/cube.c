#include "cube.h"
#include "types.h"

#if defined(_WIN32) // For GLEW on Windows
#include <windows.h>
#endif
#include <GL/glew.h> // GLEW must be first

#if defined(__APPLE__)
#include <OpenGL/gl.h> // Apple's GL header
#else
// On other systems (like Linux), glew.h itself should handle the inclusion
// of necessary GL headers or provide all definitions.
// The original file had an explicit #include <GL/gl.h> here.
// For now, I'll keep it to minimize potential side effects on non-Apple, non-Windows platforms,
// though it's often redundant after including glew.h.
#include <GL/gl.h>
#endif

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

    glBindBuffer(GL_ARRAY_BUFFER, vboVertexId);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vboColorId);
    glColorPointer(3, GL_FLOAT, 0, NULL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_QUADS, 0, 24);

    glBindBuffer(GL_ARRAY_BUFFER, vboOutlineColorId);
    glColorPointer(3, GL_FLOAT, 0, NULL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_QUADS, 0, 24);

    glPopMatrix();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawCubeDepth(Vector3 position) {
    // Note: This function assumes the depth shader program is already active
    // and appropriate uniforms (model, view, projection) are set.
    // It also assumes that the VBOs are already initialized via initCubeVBOs().

    glPushMatrix(); // Using old matrix style for consistency with drawCube
    glTranslatef(position.x + 0.5, position.y - 1.0, position.z + 0.5);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertexId);
    glVertexPointer(3, GL_FLOAT, 0, NULL); // This is legacy, for modern GL use layout qualifiers in shader

    // For depth pass, we only need to draw the geometry.
    // No color, no texture, no outline.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_QUADS, 0, 24);

    glPopMatrix();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initCubeVBOs() {
    glGenBuffers(1, &vboVertexId);
    glGenBuffers(1, &vboColorId);
    glGenBuffers(1, &vboOutlineColorId);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertexId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboColorId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboOutlineColorId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(outlineColors), outlineColors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void freeCubeVBOs() {
    glDeleteBuffers(1, &vboVertexId);
    glDeleteBuffers(1, &vboColorId);
    glDeleteBuffers(1, &vboOutlineColorId);
}
