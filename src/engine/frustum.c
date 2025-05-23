#include "frustum.h"
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#endif
#include <math.h>

static void normalizePlane(Plane* plane) {
    float mag = sqrtf(plane->x * plane->x + plane->y * plane->y + plane->z * plane->z);
    if (mag > 0.00001f) {
        plane->x /= mag;
        plane->y /= mag;
        plane->z /= mag;
        plane->w /= mag;
    }
}

void extractFrustumPlanes(Frustum* frustum) {
    GLfloat proj[16];
    GLfloat modl[16];
    GLfloat clip[16];

    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);

    clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
    clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
    clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
    clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

    clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
    clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
    clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
    clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

    clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
    clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
    clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

    frustum->planes[0].x = clip[3] + clip[0];
    frustum->planes[0].y = clip[7] + clip[4];
    frustum->planes[0].z = clip[11] + clip[8];
    frustum->planes[0].w = clip[15] + clip[12];
    normalizePlane(&frustum->planes[0]);

    frustum->planes[1].x = clip[3] - clip[0];
    frustum->planes[1].y = clip[7] - clip[4];
    frustum->planes[1].z = clip[11] - clip[8];
    frustum->planes[1].w = clip[15] - clip[12];
    normalizePlane(&frustum->planes[1]);

    frustum->planes[2].x = clip[3] + clip[1];
    frustum->planes[2].y = clip[7] + clip[5];
    frustum->planes[2].z = clip[11] + clip[9];
    frustum->planes[2].w = clip[15] + clip[13];
    normalizePlane(&frustum->planes[2]);

    frustum->planes[3].x = clip[3] - clip[1];
    frustum->planes[3].y = clip[7] - clip[5];
    frustum->planes[3].z = clip[11] - clip[9];
    frustum->planes[3].w = clip[15] - clip[13];
    normalizePlane(&frustum->planes[3]);

    frustum->planes[4].x = clip[3] + clip[2];
    frustum->planes[4].y = clip[7] + clip[6];
    frustum->planes[4].z = clip[11] + clip[10];
    frustum->planes[4].w = clip[15] + clip[14];
    normalizePlane(&frustum->planes[4]);

    frustum->planes[5].x = clip[3] - clip[2];
    frustum->planes[5].y = clip[7] - clip[6];
    frustum->planes[5].z = clip[11] - clip[10];
    frustum->planes[5].w = clip[15] - clip[14];
    normalizePlane(&frustum->planes[5]);
}

bool isAABBInFrustum(const Frustum* frustum, const Vector3* center, const Vector3* extents) {
    for (int i = 0; i < 6; i++) {
        const Plane* p = &frustum->planes[i];

        float dist_center = p->x * center->x + p->y * center->y + p->z * center->z + p->w;
        float radius_proj = extents->x * fabsf(p->x) + extents->y * fabsf(p->y) + extents->z * fabsf(p->z);
        
        if (dist_center < -radius_proj) {
            return false;
        }
    }

    return true;
}

void getCubeAABB(Vector3 basePosition, Vector3* center, Vector3* extents) {
    center->x = basePosition.x + 0.5f;
    center->y = basePosition.y - 1.0f;
    center->z = basePosition.z + 0.5f;

    extents->x = 0.5f;
    extents->y = 0.5f;
    extents->z = 0.5f;
}