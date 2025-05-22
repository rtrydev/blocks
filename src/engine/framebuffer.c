#include "framebuffer.h"
#include <stdio.h>
#include <math.h> // For floor and log2

// Helper to get max of two integers
int maxInt(int a, int b) {
    return (a > b) ? a : b;
}

int createDepthMapFBO(DepthMapFBO *depthMap, int width, int height) {
    depthMap->width = width;
    depthMap->height = height;

    // Calculate the maximum number of mipmap levels
    // The base level (level 0) is the original texture.
    // Mipmaps go down to 1x1.
    // max_level = floor(log2(max(width, height)))
    int max_dim = maxInt(width, height);
    int max_levels = 0;
    if (max_dim > 0) { // log2 is undefined for 0 or negative
        max_levels = (int)floor(log2((double)max_dim));
    }
    // Some implementations might expect max_levels to be 0 if only base level exists.
    // However, GL_TEXTURE_MAX_LEVEL specifies the index of the highest mipmap level.
    // If max_levels is 0, it means only level 0 (the base image) is used.

    // Create depth texture
    glGenTextures(1, &depthMap->depthTextureId);
    glBindTexture(GL_TEXTURE_2D, depthMap->depthTextureId);
    // Specify GL_DEPTH_COMPONENT32F for better precision if available and needed,
    // GL_DEPTH_COMPONENT is also fine.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    // Set texture parameters for mipmapping
    // Using GL_NEAREST_MIPMAP_NEAREST for minification if you want sharp transitions between mip levels,
    // or GL_LINEAR_MIPMAP_LINEAR for smoother transitions (bilinear filtering on two closest mipmaps, then linear interpolation).
    // GL_NEAREST is fine for magnification filter.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Border color for clamping
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Set the maximum number of mipmap levels for this texture
    // This is important for glGenerateMipmap to know how many levels to create.
    // If max_levels is 0, it means only the base level is used, and glGenerateMipmap might be a no-op or error.
    // However, it's generally safe to set it. If max_levels is 0, it implies only base level.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_levels);

    // Note: glGenerateMipmap() is NOT called here. It should be called after rendering to the texture.
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create FBO
    glGenFramebuffers(1, &depthMap->fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMap->fboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap->depthTextureId, 0);
    
    // We don't need to draw color, so tell OpenGL
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Error: Framebuffer is not complete!\n");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteTextures(1, &depthMap->depthTextureId);
        glDeleteFramebuffers(1, &depthMap->fboId);
        return 0; // Failure
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 1; // Success
}

void bindDepthMapFBO(const DepthMapFBO *depthMap) {
    glViewport(0, 0, depthMap->width, depthMap->height);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMap->fboId);
}

void unbindDepthMapFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Optionally, reset viewport to main window's dimensions here if needed immediately
}

void deleteDepthMapFBO(DepthMapFBO *depthMap) {
    if (depthMap->fboId) {
        glDeleteFramebuffers(1, &depthMap->fboId);
        depthMap->fboId = 0;
    }
    if (depthMap->depthTextureId) {
        glDeleteTextures(1, &depthMap->depthTextureId);
        depthMap->depthTextureId = 0;
    }
}
