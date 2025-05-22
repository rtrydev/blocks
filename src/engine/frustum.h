
#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <stdbool.h>
#include "types.h"

typedef struct {
    float x, y, z, w;
} Plane;

typedef struct {
    Plane planes[6];
} Frustum;

#include "framebuffer.h" // For DepthMapFBO
#include <GL/glew.h>     // For GLuint, GLfloat

void extractFrustumPlanes(Frustum* frustum);
bool isAABBInFrustum(const Frustum* frustum, const Vector3* center, const Vector3* extents);
void getCubeAABB(Vector3 basePosition, Vector3* center, Vector3* extents);

// HZB Functions
float readDepthFromMipmap(GLuint textureId, float u, float v, int mipLevel, int textureWidth, int textureHeight);
bool isOccludedByHZB(const Vector3* aabbMin, const Vector3* aabbMax, 
                     const DepthMapFBO* depthMapFBO, 
                     const GLfloat projectionMatrix[16], 
                     const GLfloat modelViewMatrix[16]);

#endif

