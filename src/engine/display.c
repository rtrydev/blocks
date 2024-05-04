#include "display.h"
#include "player.h"
#include "world.h"
#include "viewport.h"
#include "forces.h"
#include "gametime.h"

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

void processDisplayLoop(GLFWwindow* window) {
    int frames = 0;

    double secondStart = glfwGetTime();
    double currentTime = secondStart;

    int*** world = NULL;
    generateWorld(&world);

    while(!glfwWindowShouldClose(window))
    {
        processDeltaTime();
        currentTime = glfwGetTime();

        if (currentTime - secondStart >= 1.0) {
            printf("%d fps\n", frames);
            frames = 0;
            secondStart = currentTime;
        } else {
            frames++;
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

        drawWorld(&world);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    removeWorld(&world);
}
