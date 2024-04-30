#include "window.h"
#include "player.h"
#include <stdio.h>
#include <stdbool.h>

bool isWClicked = false;
bool isSClicked = false;
bool isDClicked = false;
bool isAClicked = false;

void keyboardButtonCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W) {
            isWClicked = false;
        }

        if (key == GLFW_KEY_S) {
            isSClicked = false;
        }

        if (key == GLFW_KEY_D) {
            isDClicked = false;
        }

        if (key == GLFW_KEY_A) {
            isAClicked = false;
        }
    }

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            isWClicked = true;
        }

        if (key == GLFW_KEY_S) {
            isSClicked = true;
        }

        if (key == GLFW_KEY_D) {
            isDClicked = true;
        }

        if (key == GLFW_KEY_A) {
            isAClicked = true;
        }

        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    playerForwardForce = 0.0;
    playerSidewayForce = 0.0;

    if (isWClicked) {
        playerForwardForce += 1.0;
    }

    if (isSClicked) {
        playerForwardForce -= 1.0;
    }

    if (isDClicked) {
        playerSidewayForce += 1.0;
    }

    if (isAClicked) {
        playerSidewayForce -= 1.0;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int x, int y) {
}

void cursorPositionCallback(GLFWwindow* window, double x, double y) {
}

GLFWwindow* initWindow(const int width, const int height)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(width, height, "Blocks", NULL, NULL);

    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowMonitor(window, NULL, width, height, 0, 0, 1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, keyboardButtonCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return window;
}