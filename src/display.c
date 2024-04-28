#include "display.h"
#include "cube.h"
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

void processViewRotationDelta(
    double deltaX, double deltaY,
    double* lookAtX, double* lookAtY, double* lookAtZ
) {
    double rotationFactor = 360.0;

    double x = deltaX / rotationFactor;
    double y = deltaY / rotationFactor;

    double longitude = x;
    double latitude = 2 * atan(exp(y)) - 3.14159265359 / 2.0;

    *lookAtX = cos(latitude) * cos(longitude);
    *lookAtY = -sin(latitude);
    *lookAtZ = cos(latitude) * sin(longitude);
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

        double positionX = 5.0;
        double positionY = 1.5;
        double positionZ = 5.0;

        processViewRotationDelta(
            currentMouseX - startX + xPositionOffset, currentMouseY - startY + yPositionOffset,
            &lookAtX, &lookAtY, &lookAtZ
        );

        gluLookAt(
            positionX, positionY, positionZ,
            lookAtX + positionX, lookAtY + positionY, lookAtZ + positionZ,
            0, 1, 0
        );

        glMatrixMode(GL_MODELVIEW_MATRIX);

        drawCube();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}