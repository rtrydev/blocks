#include "display.h"
#include "cube.h"
#include "player.h"
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

double PI = 3.14159265359;
double EPSILON = 0.0001;

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
    double forceVectorLength = sqrt(
        pow(playerForwardForce, 2) + pow(playerSidewayForce, 2)
    );

    if (forceVectorLength < EPSILON) {
        return;
    }

    double normalizedForward = playerForwardForce / forceVectorLength;
    double normalizedSideway = playerSidewayForce / forceVectorLength;

    playerPositionX += normalizedForward * lookAtX * 0.005;
    playerPositionZ += normalizedForward * lookAtZ * 0.005;

    playerPositionX += normalizedSideway * (lookAtX * cos(PI / 2.0) - lookAtZ * sin(PI / 2.0)) * 0.005;
    playerPositionZ += normalizedSideway * (lookAtX * sin(PI / 2.0) + lookAtZ * cos(PI / 2.0)) * 0.005;
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

    double xPositionOffset = -876;
    double yPositionOffset = 79;

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
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        // glClearColor(0.5294, 0.8078, 0.9215, 1.0);
        glClearColor(0, 0, 0, 1.0);
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
            playerPositionX, playerPositionY, playerPositionZ,
            lookAtX + playerPositionX, lookAtY + playerPositionY, lookAtZ + playerPositionZ,
            0, 1, 0
        );

        glMatrixMode(GL_MODELVIEW_MATRIX);

        drawCube();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}