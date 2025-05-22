
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

void extractFrustumPlanes(Frustum* frustum);
bool isAABBInFrustum(const Frustum* frustum, const Vector3* center, const Vector3* extents);
void getCubeAABB(Vector3 basePosition, Vector3* center, Vector3* extents);

#endif

