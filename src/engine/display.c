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
#else
#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glu.h>
#include <GL/glut.h> // For glutBitmapCharacter
#endif
#if defined(__APPLE__)
#include <GLUT/glut.h> // For glutBitmapCharacter on macOS
#endif

void processDisplayLoop(GLFWwindow* window) {
    double lastFpsTime = glfwGetTime();
    int frameCount = 0;
    char fpsText[32];
    sprintf(fpsText, "FPS: N/A"); // Initial text

    generateWorld();

    while(!glfwWindowShouldClose(window))
    {
        processDeltaTime();

        GLint windowWidth, windowHeight;

        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glClearColor(0.5294, 0.8078, 0.9215, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective(60, (double)windowWidth / (double)windowHeight, 0.1, 100);
        
        addForcesBasedOnInputs();
        adjustForcesBasedOnCollision();
        processForces();

        playerFollowViewport();

        Vector3 rotation = getViewportRotation();
        Vector3 position = getViewportPosition();
        PlayerState playerState = getPlayerState();

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

        Frustum viewFrustum;
        extractFrustumPlanes(&viewFrustum);

        drawWorld(&viewFrustum);

        // FPS calculation
        frameCount++;
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - lastFpsTime;

        if (elapsedTime >= 1.0) { // If one second has passed
            double fps = (double)frameCount / elapsedTime;
            sprintf(fpsText, "FPS: %.2f", fps);
            frameCount = 0;
            lastFpsTime = currentTime;
        }

        // Setup for UI rendering
        // GLint windowWidth, windowHeight; are already fetched earlier in the loop
        setupOrthographicProjection(windowWidth, windowHeight);

        // Render FPS text (e.g., at top-left)
        // Y-coordinate needs to be adjusted based on windowHeight for top-left.
        // If (0,0) is bottom-left, then top-left is (x_offset, windowHeight - y_offset).
        renderText(10.0f, windowHeight - 20.0f, fpsText, 1.0f, 1.0f, 0.0f); // x, y, text, r, g, b (yellow)

        // Restore perspective projection
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
