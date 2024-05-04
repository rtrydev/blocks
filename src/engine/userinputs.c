#include "userinputs.h"
#include "viewport.h"
#include "constants.h"
#include "gametime.h"

#include <math.h>
#include <stdlib.h>

double startViewX = 0.0;
double startViewY = 0.0;
bool startViewInitialized = false;

double lastViewXPosition = 0.0;
double lastViewYPosition = 0.0;

double currentClampedX = 0.0;
double currentClampedY = 0.0;

InputState currentInputState = {
	.UP_ACTIVE = false,
	.DOWN_ACTIVE = false,
    .RIGHT_ACTIVE = false,
	.LEFT_ACTIVE = false,
	.JUMP_ACTIVE = false
};

InputState getInputState() {
    return currentInputState;
}

void processKeyboardButtonActions(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W) {
            currentInputState.UP_ACTIVE = false;
        }

        if (key == GLFW_KEY_S) {
            currentInputState.DOWN_ACTIVE = false;
        }

        if (key == GLFW_KEY_D) {
            currentInputState.RIGHT_ACTIVE = false;
        }

        if (key == GLFW_KEY_A) {
            currentInputState.LEFT_ACTIVE = false;
        }

        if (key == GLFW_KEY_SPACE) {
            currentInputState.JUMP_ACTIVE = false;
        }
    }

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            currentInputState.UP_ACTIVE = true;
        }

        if (key == GLFW_KEY_S) {
            currentInputState.DOWN_ACTIVE = true;
        }

        if (key == GLFW_KEY_D) {
            currentInputState.RIGHT_ACTIVE = true;
        }

        if (key == GLFW_KEY_A) {
            currentInputState.LEFT_ACTIVE = true;
        }

        if (key == GLFW_KEY_SPACE) {
            currentInputState.JUMP_ACTIVE = true;
        }

        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
}

void processMouseButtonActions(GLFWwindow* window, int button, int x, int y) {

}

void processMouseMoveActions(GLFWwindow* window, double x, double y) {
    if (!startViewInitialized) {
        startViewX = x;
        startViewY = y;

        startViewInitialized = true;
    }

    x -= startViewX;
    y -= startViewY;

    double viewDeltaX = x - lastViewXPosition;
    double viewDeltaY = y - lastViewYPosition;

    lastViewXPosition = x;
    lastViewYPosition = y;

    if (currentClampedX + viewDeltaX > X_VIEW_CLAMP) {
        currentClampedX = viewDeltaY + currentClampedX - 2 * X_VIEW_CLAMP;
    }
    else if (currentClampedX + viewDeltaX < -X_VIEW_CLAMP) {
        currentClampedX = viewDeltaX + currentClampedX + 2 * X_VIEW_CLAMP;
    }
    else {
        currentClampedX = currentClampedX + viewDeltaX;
    }

    currentClampedY = min(max(currentClampedY + viewDeltaY, -Y_VIEW_CLAMP), Y_VIEW_CLAMP);

    double longitude = currentClampedX / ROTATION_FACTOR;
    double latitude = 2 * atan(exp(currentClampedY / ROTATION_FACTOR)) - PI / 2.0;

    Vector3 rotation;

    rotation.x = cos(latitude) * cos(longitude);
    rotation.y = -currentClampedY / ROTATION_FACTOR;
    rotation.z = cos(latitude) * sin(longitude);

    setViewportRotation(rotation);
}
