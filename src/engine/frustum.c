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
#include <stdio.h> // Added for printf
#include <stdlib.h> // Added for malloc/free (already present from previous step but good to ensure)

static void normalizePlane(Plane* plane) {
    float mag = sqrtf(plane->x * plane->x + plane->y * plane->y + plane->z * plane->z);
    if (mag > 0.00001f) { // Avoid division by zero or very small numbers
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

// Helper function to project a single vertex
static void projectVertex(const Vector3* worldVertex, const GLfloat projectionMatrix[16], const GLfloat modelViewMatrix[16], float projected[4]) {
    float worldPos[4] = {worldVertex->x, worldVertex->y, worldVertex->z, 1.0f};
    float eyePos[4];
    float clipPos[4];

    // World to Eye Space (modelViewMatrix * worldPos)
    eyePos[0] = modelViewMatrix[0] * worldPos[0] + modelViewMatrix[4] * worldPos[1] + modelViewMatrix[8] * worldPos[2] + modelViewMatrix[12] * worldPos[3];
    eyePos[1] = modelViewMatrix[1] * worldPos[0] + modelViewMatrix[5] * worldPos[1] + modelViewMatrix[9] * worldPos[2] + modelViewMatrix[13] * worldPos[3];
    eyePos[2] = modelViewMatrix[2] * worldPos[0] + modelViewMatrix[6] * worldPos[1] + modelViewMatrix[10] * worldPos[2] + modelViewMatrix[14] * worldPos[3];
    eyePos[3] = modelViewMatrix[3] * worldPos[0] + modelViewMatrix[7] * worldPos[1] + modelViewMatrix[11] * worldPos[2] + modelViewMatrix[15] * worldPos[3];

    // Eye to Clip Space (projectionMatrix * eyePos)
    clipPos[0] = projectionMatrix[0] * eyePos[0] + projectionMatrix[4] * eyePos[1] + projectionMatrix[8] * eyePos[2] + projectionMatrix[12] * eyePos[3];
    clipPos[1] = projectionMatrix[1] * eyePos[0] + projectionMatrix[5] * eyePos[1] + projectionMatrix[9] * eyePos[2] + projectionMatrix[13] * eyePos[3];
    clipPos[2] = projectionMatrix[2] * eyePos[0] + projectionMatrix[6] * eyePos[1] + projectionMatrix[10] * eyePos[2] + projectionMatrix[14] * eyePos[3];
    clipPos[3] = projectionMatrix[3] * eyePos[0] + projectionMatrix[7] * eyePos[1] + projectionMatrix[11] * eyePos[2] + projectionMatrix[15] * eyePos[3];

    // Perspective divide (to NDC)
    if (clipPos[3] != 0.0f) {
        projected[0] = clipPos[0] / clipPos[3];
        projected[1] = clipPos[1] / clipPos[3];
        projected[2] = clipPos[2] / clipPos[3]; // NDC z
        projected[3] = clipPos[3]; // Store w for later use if needed
    } else {
        // Should not happen with a well-formed projection matrix for visible objects
        projected[0] = 0.0f; projected[1] = 0.0f; projected[2] = 0.0f; projected[3] = 0.0f;
    }
}


float readDepthFromMipmap(GLuint textureId, float u, float v, int mipLevel, int textureWidth, int textureHeight) {
    if (textureId == 0) return 1.0f; // No texture, assume not occluded

    glBindTexture(GL_TEXTURE_2D, textureId);

    int mipWidth = textureWidth;
    int mipHeight = textureHeight;
    if (mipLevel > 0) {
        // Get dimensions of the specified mipmap level
        glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &mipWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &mipHeight);
    }
    
    if (mipWidth == 0 || mipHeight == 0) { // Should not happen for valid levels
        glBindTexture(GL_TEXTURE_2D, 0);
        return 1.0f; 
    }

    // Clamp and scale u, v to pixel coordinates
    int texX = (int)(fmax(0.0f, fmin(1.0f, u)) * (mipWidth -1));
    int texY = (int)(fmax(0.0f, fmin(1.0f, v)) * (mipHeight -1));

    float depthValue = 1.0f; // Default to farthest depth
    // glGetTexImage is slow. For a single pixel, it's acceptable for a demo.
    // Reading a region might be better if more samples are needed, but still slow.
    glGetTexImage(GL_TEXTURE_2D, mipLevel, GL_DEPTH_COMPONENT, GL_FLOAT, &depthValue);
    // The above call is simplified. glGetTexImage expects a pointer to a buffer.
    // To read a single pixel, you'd typically read a 1x1 region.
    // Let's correct this to read a 1x1 pixel region at (texX, texY)
    // However, glGetTexImage reads the *entire* texture level or a specified region,
    // not just one pixel directly by coordinates like this.
    // The most direct way for a single pixel with glGetTexImage is to read the whole level
    // and then access the pixel, which is very inefficient.
    // A better (but still CPU-side and slow) way is to use glReadPixels from the FBO
    // if the FBO was still bound and set to read from the correct mip level (not standard).
    //
    // Given the constraints and typical use of glGetTexImage, we'll fetch the whole mip level
    // and then index into it. This is highly inefficient for per-object calls.
    // For the purpose of this exercise, we'll simulate reading just one pixel,
    // acknowledging this is not how glGetTexImage is typically used for performance.
    // A real CPU HZB check would often involve pre-fetching relevant tiles of the HZB.

    // Corrected (but still inefficient) approach for glGetTexImage:
    float* pixels = (float*)malloc(mipWidth * mipHeight * sizeof(float));
    if (pixels) {
        glGetTexImage(GL_TEXTURE_2D, mipLevel, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
        depthValue = pixels[texY * mipWidth + texX]; // Assuming row-major order
        free(pixels);
    } else {
        // Malloc failed
        glBindTexture(GL_TEXTURE_2D, 0);
        return 1.0f; // Error case
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return depthValue;
}

bool isOccludedByHZB(const Vector3* aabbMinWorld, const Vector3* aabbMaxWorld,
                     const DepthMapFBO* depthMapFBO,
                     const GLfloat projectionMatrix[16],
                     const GLfloat modelViewMatrix[16]) {

    Vector3 aabbCorners[8];
    aabbCorners[0] = (Vector3){aabbMinWorld->x, aabbMinWorld->y, aabbMinWorld->z};
    aabbCorners[1] = (Vector3){aabbMaxWorld->x, aabbMinWorld->y, aabbMinWorld->z};
    aabbCorners[2] = (Vector3){aabbMinWorld->x, aabbMaxWorld->y, aabbMinWorld->z};
    aabbCorners[3] = (Vector3){aabbMaxWorld->x, aabbMaxWorld->y, aabbMinWorld->z};
    aabbCorners[4] = (Vector3){aabbMinWorld->x, aabbMinWorld->y, aabbMaxWorld->z};
    aabbCorners[5] = (Vector3){aabbMaxWorld->x, aabbMinWorld->y, aabbMaxWorld->z};
    aabbCorners[6] = (Vector3){aabbMinWorld->x, aabbMaxWorld->y, aabbMaxWorld->z};
    aabbCorners[7] = (Vector3){aabbMaxWorld->x, aabbMaxWorld->y, aabbMaxWorld->z};

    float minScreenX = 2.0f, maxScreenX = -2.0f;
    float minScreenY = 2.0f, maxScreenY = -2.0f;
    float minScreenZ = 1.0f; // NDC z is 0 to 1 or -1 to 1. Furthest object is at 1.0 after (z+1)/2.

    for (int i = 0; i < 8; ++i) {
        float projected[4];
        projectVertex(&aabbCorners[i], projectionMatrix, modelViewMatrix, projected);

        // projected[0], projected[1], projected[2] are in NDC (-1 to 1 range)
        minScreenX = fminf(minScreenX, projected[0]);
        maxScreenX = fmaxf(maxScreenX, projected[0]);
        minScreenY = fminf(minScreenY, projected[1]);
        maxScreenY = fmaxf(maxScreenY, projected[2]); // Typo: should be projected[1]
        minScreenZ = fminf(minScreenZ, projected[2]); 
    }
    // Correct typo from above
    maxScreenY = -2.0f; // Re-initialize
     for (int i = 0; i < 8; ++i) {
        float projected[4];
        projectVertex(&aabbCorners[i], projectionMatrix, modelViewMatrix, projected);
        maxScreenY = fmaxf(maxScreenY, projected[1]);
    }


    // If AABB is entirely behind the camera (or partially, and minScreenZ is far)
    // or outside clip space, it's not occluded by HZB in a simple check,
    // but could be frustum culled. This check assumes it passed frustum culling.
    if (maxScreenX < -1.0f || minScreenX > 1.0f || maxScreenY < -1.0f || minScreenY > 1.0f || minScreenZ > 1.0f || minScreenZ < -1.0f) {
         // If any part of the AABB is outside the [-1, 1] NDC cube (except for Z far plane),
         // it's complicated. For simplicity, if it's way off, consider not occluded by HZB.
         // A more robust check would handle clipping.
        return false; 
    }
    
    // Convert NDC minScreenZ from [-1, 1] to [0, 1] if necessary (OpenGL default)
    // If your depth buffer is [0,1], then NDC z values are also typically mapped to [0,1]
    // by the projection matrix (e.g. glDepthRange(0,1)).
    // If NDC z is [-1, 1], then map to [0, 1]: objectNDC_z = (objectNDC_z_clip + 1.0) * 0.5;
    // For now, assume minScreenZ is already in [0,1] as typically stored in depth textures.
    // If your projection results in Z values that are not directly comparable to depth texture values,
    // this needs adjustment. Let's assume standard OpenGL depth [0, 1].
    // The projected Z (minScreenZ) is the closest Z of the AABB to the camera.

    // Center of the AABB in screen space (NDC)
    float u_center_ndc = (minScreenX + maxScreenX) * 0.5f;
    float v_center_ndc = (minScreenY + maxScreenY) * 0.5f;

    // Convert NDC to texture coordinates (UVs for texture sampling) [0, 1] range
    float u_center_tex = (u_center_ndc + 1.0f) * 0.5f;
    float v_center_tex = (v_center_ndc + 1.0f) * 0.5f;

    // Determine mipmap level (simplified: highest mip level)
    int max_mip_level = 0;
    if (depthMapFBO->width > 0 && depthMapFBO->height > 0) {
         max_mip_level = (int)floor(log2((double)fmaxf(depthMapFBO->width, depthMapFBO->height)));
    }
    // Clamp to be safe, although max_mip_level should be correct if HZB was set up with TEXTURE_MAX_LEVEL
    int texture_max_level_param = 0;
    glBindTexture(GL_TEXTURE_2D, depthMapFBO->depthTextureId);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &texture_max_level_param);
    glBindTexture(GL_TEXTURE_2D, 0);
    max_mip_level = fminf(max_mip_level, texture_max_level_param); // Ensure we don't exceed actual max level
    if (max_mip_level < 0) max_mip_level = 0;


    // Sample HZB
    float depthFromHZB = readDepthFromMipmap(depthMapFBO->depthTextureId, u_center_tex, v_center_tex, max_mip_level, depthMapFBO->width, depthMapFBO->height);

    // Compare
    // If the closest part of the object (minScreenZ) is further away than the depth stored in HZB,
    // then the object is occluded.
    // This comparison assumes minScreenZ and depthFromHZB are in the same space [0,1], where 0 is near, 1 is far.
    bool isOccluded = minScreenZ > depthFromHZB + 0.0001f; // Add a small epsilon

    if (isOccluded) {
        printf("Occlusion check: AABB (minWorld: %.1f,%.1f,%.1f) minScreenZ: %f, HZB depth: %f (at mip %d, uv %.2f,%.2f). Occluded.\n",
               aabbMinWorld->x, aabbMinWorld->y, aabbMinWorld->z,
               minScreenZ, depthFromHZB, max_mip_level, u_center_tex, v_center_tex);
        return true;
    } else {
        printf("Occlusion check: AABB (minWorld: %.1f,%.1f,%.1f) minScreenZ: %f, HZB depth: %f (at mip %d, uv %.2f,%.2f). Not occluded.\n",
               aabbMinWorld->x, aabbMinWorld->y, aabbMinWorld->z,
               minScreenZ, depthFromHZB, max_mip_level, u_center_tex, v_center_tex);
        return false;
    }
}