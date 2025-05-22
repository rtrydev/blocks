#include "display.h"
#include "player.h"
#include "world.h"
#include "viewport.h"
#include "forces.h"
#include "gametime.h"
#include "frustum.h"

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#if defined(__APPLE__)
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glu.h>
#include <GL/glut.h>
#endif

// Added for new parameters
#include "framebuffer.h"
#include "shader.h" // Though not directly used, it's good practice if display needs shader interactions

void processDisplayLoop(GLFWwindow* window, GLuint depthShaderProgram, const DepthMapFBO* depthMapFBO) {
    double lastFpsTime = glfwGetTime();
    int frameCount = 0;
    char fpsText[32];
    sprintf(fpsText, "FPS: N/A");

    generateWorld();

    while(!glfwWindowShouldClose(window))
    {
        processDeltaTime();

        // --- Depth Pass ---
        glUseProgram(depthShaderProgram); // Use depth shader
        bindDepthMapFBO(depthMapFBO);     // Bind FBO, sets viewport to FBO dimensions
        glClear(GL_DEPTH_BUFFER_BIT);     // Clear only depth buffer
        glEnable(GL_DEPTH_TEST);          // Ensure depth testing is enabled

        // Setup camera for depth pass (e.g., from light's perspective or player's)
        // For this iteration, we'll use the player's view, similar to the main pass.
        // Note: The projection matrix for the depth pass might need to be different
        // (e.g., tighter frustum for shadows) depending on the application.
        // The view matrix would be from the light's point of view for shadow mapping.
        // Here, we are just creating a depth map from the player's view.

        glMatrixMode(GL_PROJECTION_MATRIX); // Still using fixed-function for matrix setup
        glLoadIdentity();
        // Use FBO dimensions for aspect ratio if perspective is different, or keep main window's
        gluPerspective(60, (double)depthMapFBO->width / (double)depthMapFBO->height, 0.1, 100); // Example perspective

        PlayerState playerStateDepth = getPlayerState(); // Get current player state
        Vector3 rotationDepth = getViewportRotation();   // Assumes these give suitable view parameters
        Vector3 positionDepth = getViewportPosition();

        // Set view matrix for depth pass
        gluLookAt(
            positionDepth.x,
            positionDepth.y + playerStateDepth.height - 1.0,
            positionDepth.z,
            rotationDepth.x + positionDepth.x,
            rotationDepth.y + positionDepth.y + playerStateDepth.height - 1.0,
            rotationDepth.z + positionDepth.z,
            0, 1, 0
        );
        
        glMatrixMode(GL_MODELVIEW_MATRIX); // Switch to modelview for object transformations
        // Note: if glUseProgram is active, fixed-function matrix operations (glLoadIdentity, glTranslate, etc.
        // in drawCubeDepth) affect the legacy matrix stack, which is NOT automatically used by shaders.
        // The depth_vertex.glsl shader expects uniforms `model`, `view`, `projection`.
        // These are NOT being set here. This is a known limitation from the previous step.
        // The depth texture will still be generated based on geometry rasterization,
        // but the shader's transformations are not being utilized as intended.

        Frustum depthFrustum; // Frustum for the depth pass
        extractFrustumPlanes(&depthFrustum); // Recalculate for the depth pass camera
        
        drawWorldDepth(&depthFrustum); // Render world for depth

        // Generate Mipmaps for the depth texture
        // The FBO should still be bound, or at least the texture shouldn't be in use by another FBO.
        // It's common to do this right after rendering to the texture and before unbinding the FBO,
        // or immediately after unbinding the FBO but before the texture is used for sampling.
        glBindTexture(GL_TEXTURE_2D, depthMapFBO->depthTextureId);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture after mipmap generation

        unbindDepthMapFBO();       // Unbind FBO, viewport reset happens in main pass
        glUseProgram(0);           // Unbind shader program


        // --- Main Render Pass ---
        GLint windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight); // Reset viewport to main window

        glClearColor(0.5294, 0.8078, 0.9215, 1.0); // Set clear color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth

        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective(60, (double)windowWidth / (double)windowHeight, 0.1, 100); // Main projection

        addForcesBasedOnInputs();
        adjustForcesBasedOnCollision();
        processForces();

        playerFollowViewport();

        Vector3 rotation = getViewportRotation(); // Main camera rotation
        Vector3 position = getViewportPosition(); // Main camera position
        PlayerState playerState = getPlayerState(); // Main player state

        gluLookAt(
            position.x,
            position.y + playerState.height - 1.0,
            position.z,
            rotation.x + position.x,
            rotation.y + position.y + playerState.height - 1.0,
            rotation.z + position.z,
            0, 1, 0
        );

        glMatrixMode(GL_MODELVIEW_MATRIX);

        Frustum viewFrustum; // Frustum for the main pass
        extractFrustumPlanes(&viewFrustum); // Recalculate for main pass camera

        // Modified call to include depthMapFBO
        drawWorld(&viewFrustum, depthMapFBO); 

        frameCount++;
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - lastFpsTime;

        if (elapsedTime >= 1.0) {
            double fps = (double)frameCount / elapsedTime;
            sprintf(fpsText, "FPS: %.2f", fps);
            frameCount = 0;
            lastFpsTime = currentTime;
        }

        // UI Rendering (same as before)
        setupOrthographicProjection(windowWidth, windowHeight);
        renderText(10.0f, windowHeight - 20.0f, fpsText, 1.0f, 1.0f, 0.0f);
        restorePerspectiveProjection();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    removeWorld();
}

void setupOrthographicProjection(int windowWidth, int windowHeight) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void restorePerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void renderText(float x, float y, char *string, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    char *c;
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}
