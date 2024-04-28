#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "display.h"

int main(void) {
    GLFWwindow* window = initWindow(1280, 720);

    if (window != NULL) {
        processDisplayLoop(window);
    }

    glfwTerminate();
    return 0;
}
