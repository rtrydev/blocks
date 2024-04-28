#include "cube.h"
#include <GL/gl.h>

void drawCube() {
    GLfloat vertices[] =
    {
        -1, -1, -1,   -1, -1,  1,   -1,  1,  1,   -1,  1, -1,
        1, -1, -1,    1, -1,  1,    1,  1,  1,    1,  1, -1,
        -1, -1, -1,   -1, -1,  1,    1, -1,  1,    1, -1, -1,
        -1,  1, -1,   -1,  1,  1,    1,  1,  1,    1,  1, -1,
        -1, -1, -1,   -1,  1, -1,    1,  1, -1,    1, -1, -1,
        -1, -1,  1,   -1,  1,  1,    1,  1,  1,    1, -1,  1
    };

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

    /* We have a color array and a vertex array */
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_QUADS, 0, 24);

    glColorPointer(3, GL_FLOAT, 0, outlineColors);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_QUADS, 0, 24);

    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glRotatef(30, 0.0f, 1.0f, 0.0f);

    glTranslatef(3, -3, -3); 

    /* Cleanup states */
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}