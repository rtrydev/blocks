#include "display.h"
#include "cube.h"
#include "player.h"
#include "world.h"

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

double PI = 3.14159265359;
double EPSILON = 0.0001;
double DELTA_TIME_FACTOR = 100000.0;

long deltaTime = -1;
long previousTime = -1;

double previousPlayerPositionY;

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
    double lookAtX, double lookAtY, double lookAtZ,
    int**** worldStructure
) {
    struct timespec timeFetch;

#if defined(_WIN32)
    static LARGE_INTEGER ticksPerSec;
    LARGE_INTEGER ticks;

    if (!ticksPerSec.QuadPart) {
        QueryPerformanceFrequency(&ticksPerSec);
        if (!ticksPerSec.QuadPart) {
            errno = ENOTSUP;
            return -1;
        }
    }

    QueryPerformanceCounter(&ticks);

    timeFetch.tv_sec = (long)(ticks.QuadPart / ticksPerSec.QuadPart);
    timeFetch.tv_nsec = (long)(((ticks.QuadPart % ticksPerSec.QuadPart) * 1000 * 1000 * 1000) / ticksPerSec.QuadPart);
#else
    clock_gettime(CLOCK_MONOTONIC, &timeFetch);
#endif

    long currentTime = timeFetch.tv_sec * 1000 * 1000 + timeFetch.tv_nsec / 1000;

    if (previousTime == -1) {
        previousTime = currentTime;
    }

    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    if (playerInAir && playerPositionY == previousPlayerPositionY) {
        playerInAir = false;
        playerUpForce = -1.0;
    }

    if (!playerInAir && playerPositionY != previousPlayerPositionY) {
        playerInAir = true;
    }

    previousPlayerPositionY = playerPositionY;

    if (playerInAir && playerUpForce > -15.0) {
        playerUpForce -= 0.2 * deltaTime / DELTA_TIME_FACTOR;
    }

    bool isCollisionX = false;
    bool isCollisionY = false;
    bool isCollisionZ = false;

    double forceVectorLength = sqrt(pow(playerForwardForce, 2) + pow(playerSidewayForce, 2));

    double normalizedForward = playerSpeed * playerForwardForce / forceVectorLength;
    double normalizedSideway = playerSpeed * playerSidewayForce / forceVectorLength;

    double directionVectorLength = sqrt(pow(lookAtX, 2) + pow(lookAtZ, 2));
    double normalizedLookAtX = lookAtX / directionVectorLength;
    double normalizedLookAtZ = lookAtZ / directionVectorLength;

    double yDirection = 0.0;

    if (playerUpForce > EPSILON || playerUpForce < EPSILON) {
        yDirection = playerUpForce > 0.0 ? 1.0 : -1.0;
    }

    double xPositionChange = 0.0;
    double zPositionChange = 0.0;

    if (forceVectorLength > EPSILON) {
        xPositionChange =
            normalizedForward * normalizedLookAtX * deltaTime / DELTA_TIME_FACTOR
            + normalizedSideway * (normalizedLookAtX * cos(PI / 2.0) - normalizedLookAtZ * sin(PI / 2.0)) * deltaTime / DELTA_TIME_FACTOR;

        zPositionChange =
            normalizedForward * normalizedLookAtZ * deltaTime / DELTA_TIME_FACTOR
            + normalizedSideway * (normalizedLookAtX * sin(PI / 2.0) + normalizedLookAtZ * cos(PI / 2.0)) * deltaTime / DELTA_TIME_FACTOR;
    }

    searchForCollision(
        playerPositionX, playerPositionY, playerPositionZ,
        xPositionChange, yDirection, zPositionChange,
        &isCollisionX, &isCollisionY, &isCollisionZ,
        worldStructure
    );

    if (!isCollisionX && forceVectorLength > EPSILON) {
        playerPositionX += xPositionChange;
    }

    if (!isCollisionZ && forceVectorLength > EPSILON) {
        playerPositionZ += zPositionChange;
    }

    if (!isCollisionY) {
        playerPositionY += playerSpeed * playerUpForce * deltaTime / DELTA_TIME_FACTOR;
    }
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

    previousPlayerPositionY = playerPositionY;
    playerUpForce = -1.0;

    int*** world = NULL;
    generateWorld(&world);

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
            lookAtX, lookAtY, lookAtZ,
            &world
        );

        gluLookAt(
            playerPositionX, playerPositionY + playerHeight, playerPositionZ,
            lookAtX + playerPositionX, lookAtY + playerPositionY + playerHeight, lookAtZ + playerPositionZ,
            0, 1, 0
        );

        glMatrixMode(GL_MODELVIEW_MATRIX);

        drawWorld(&world);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    removeWorld(&world);
}
