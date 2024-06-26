#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "engine/window.h"
#include "engine/display.h"

int main(void) {
    GLFWwindow* window = initWindow(1280, 720);

    if (window != NULL) {
        processDisplayLoop(window);
    }

    glfwTerminate();
    return 0;
}
