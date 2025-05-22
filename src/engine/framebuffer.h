#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>

typedef struct {
    GLuint fboId;
    GLuint depthTextureId;
    int width;
    int height;
} DepthMapFBO;

int createDepthMapFBO(DepthMapFBO *depthMap, int width, int height);
void bindDepthMapFBO(const DepthMapFBO *depthMap);
void unbindDepthMapFBO();
void deleteDepthMapFBO(DepthMapFBO *depthMap);

#endif // FRAMEBUFFER_H
