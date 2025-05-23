#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include "engine/window.h"
#include "engine/display.h"
#include "engine/cube.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    GLFWwindow* window = initWindow(1280, 720);

    if (window != NULL) {
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
        printf("OpenGL Renderer: %s\n", (const char*)glGetString(GL_RENDERER));
        printf("OpenGL Version: %s\n", (const char*)glGetString(GL_VERSION));

        initCubeVBOs();

        // Registering callbacks from userinputs.c
        // Note: display.c calls glfwSetKeyCallback and glfwSetCursorPosCallback
        // We need to ensure those are called, or consolidate registration here.
        // For now, assuming display.c's calls are sufficient for key/mouse position
        // and we only need to add the mouse button callback here.
        // If processDisplayLoop sets them, this might be redundant or overwritten
        // depending on the order.
        // Looking at display.c, it indeed sets key and cursor pos callbacks.
        // So we just add the mouse button callback.
        glfwSetMouseButtonCallback(window, processMouseButtonActions);

        processDisplayLoop(window);
        freeCubeVBOs();

        glfwDestroyWindow(window);
    }
    else {
        fprintf(stderr, "initWindow failed, terminating.\n");
    }

    glfwTerminate();
    return 0;
}