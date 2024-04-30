#include "display.h"
#include "cube.h"
#include "player.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#if defined(__APPLE__)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

double PI = 3.14159265359;
double EPSILON = 0.0001;
double DELTA_TIME_FACTOR = 100000.0;

long deltaTime = -1;
long previousTime = -1;

void processViewRotationDelta(
    double deltaX, double deltaY,
    double* lookAtX, double* lookAtY, double* lookAtZ
) {
    double rotationFactor = 360.0;

    double x = deltaX / rotationFactor;
    double y = deltaY / rotationFactor;

    double longitude = x;
    double latitude = 2 * atan(exp(y)) - PI / 2.0;

    *lookAtX = cos(latitude) * cos(longitude);
    *lookAtY = -sin(latitude);
    *lookAtZ = cos(latitude) * sin(longitude);
}

void processPlayerPositionChange(
    double lookAtX, double lookAtY, double lookAtZ
) {
    struct timespec timeFetch;
    clock_gettime(CLOCK_MONOTONIC, &timeFetch);

    long currentTime = timeFetch.tv_sec * 1000 * 1000 + timeFetch.tv_nsec / 1000;

    if (previousTime == -1) {
        previousTime = currentTime;
    }

    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    double forceVectorLength = sqrt(
        pow(playerForwardForce, 2) + pow(playerSidewayForce, 2)
    );

    if (forceVectorLength < EPSILON) {
        return;
    }

    double normalizedForward = playerSpeed * playerForwardForce / forceVectorLength;
    double normalizedSideway = playerSpeed * playerSidewayForce / forceVectorLength;

    playerPositionX += normalizedForward * lookAtX * deltaTime / DELTA_TIME_FACTOR;
    playerPositionZ += normalizedForward * lookAtZ * deltaTime / DELTA_TIME_FACTOR;

    playerPositionX += normalizedSideway * (lookAtX * cos(PI / 2.0) - lookAtZ * sin(PI / 2.0)) * deltaTime / DELTA_TIME_FACTOR;
    playerPositionZ += normalizedSideway * (lookAtX * sin(PI / 2.0) + lookAtZ * cos(PI / 2.0)) * deltaTime / DELTA_TIME_FACTOR;
}

void processDisplayLoop(GLFWwindow* window) {
    int mousePositionInitializedState = 0;

    int frames = 0;

    double secondStart = glfwGetTime();
    double currentTime = secondStart;
    double currentMouseX = 0.0;
    double currentMouseY = 0.0;

    double lookAtX = 0.0;
    double lookAtY = 0.0;
    double lookAtZ = 0.0;

    double startX = 0.0;
    double startY = 0.0;

    double xPositionOffset = 0.0;
    double yPositionOffset = 0.0;

    while(!glfwWindowShouldClose(window))
    {
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

        if (mousePositionInitializedState < 2) {
            glfwGetCursorPos(window, &startX, &startY);
            mousePositionInitializedState++;
        }

        glfwGetCursorPos(window, &currentMouseX, &currentMouseY);

        processViewRotationDelta(
            currentMouseX - startX + xPositionOffset, currentMouseY - startY + yPositionOffset,
            &lookAtX, &lookAtY, &lookAtZ
        );
        processPlayerPositionChange(
            lookAtX, lookAtY, lookAtZ
        );

        gluLookAt(
            playerPositionX, playerPositionY + playerHeight, playerPositionZ,
            lookAtX + playerPositionX, lookAtY + playerPositionY + playerHeight, lookAtZ + playerPositionZ,
            0, 1, 0
        );

        glMatrixMode(GL_MODELVIEW_MATRIX);

        drawCube(2.0, 1.0, 2.0);
        drawCube(1.0, 1.0, 2.0);
        drawCube(2.0, 1.0, 3.0);
        drawCube(2.0, 2.0, 2.0);
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                drawCube((double)i - 5, 0.0, (double)j - 5);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
