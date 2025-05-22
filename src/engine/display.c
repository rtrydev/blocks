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
#endif

// It's good practice to ensure GLFW is included if using its functions directly.
// However, it's likely included via "display.h" or another engine header
// if glfwGetTime() was already in use. For this exercise, I'll assume it's accessible.
// #include <GLFW/glfw3.h> // Uncomment if direct include is necessary

void processDisplayLoop(GLFWwindow* window) {
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    double currentTime = lastTime; // Initialize currentTime

    generateWorld();

    while(!glfwWindowShouldClose(window))
    {
        processDeltaTime();
        currentTime = glfwGetTime(); // Update current time each frame

        // Measure speed
        nbFrames++;
        if (currentTime - lastTime >= 1.0 ){ // If last prinf & HReset was more than 1 sec ago
            // printf and reset timer
            printf("%f ms/frame (%d FPS)\n", 1000.0 / (double)nbFrames, nbFrames);
            nbFrames = 0;
            lastTime += 1.0;
        }

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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    removeWorld();
}
