#include "cube.h"
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

void drawCube(double positionX, double positionY, double positionZ) {
    GLfloat vertices[] =
    {
        -1, -1, -1,   -1, -1,  1,   -1,  1,  1,   -1,  1, -1,
        1, -1, -1,    1, -1,  1,    1,  1,  1,    1,  1, -1,
        -1, -1, -1,   -1, -1,  1,    1, -1,  1,    1, -1, -1,
        -1,  1, -1,   -1,  1,  1,    1,  1,  1,    1,  1, -1,
        -1, -1, -1,   -1,  1, -1,    1,  1, -1,    1, -1, -1,
        -1, -1,  1,   -1,  1,  1,    1,  1,  1,    1, -1,  1
    };

    for (int i = 0; i < 72; i++) {
        vertices[i] *= 0.5;
    }

    GLfloat colors[] =
    {
        0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
        0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
        0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
        0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
        0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,
        0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0,   0, 0.2, 0
    };

    GLfloat outlineColors[] =
    {
        1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
        1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
        1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
        1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
        1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1,
        1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1
    };

    glPushMatrix();
    glTranslatef(positionX, positionY - 1.0, positionZ);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_QUADS, 0, 24);

    glColorPointer(3, GL_FLOAT, 0, outlineColors);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_QUADS, 0, 24);

    glPopMatrix();

    /* Cleanup states */
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}
