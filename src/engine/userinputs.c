#include "userinputs.h"
#include "viewport.h"
#include "constants.h"
#include "gametime.h"
#include "world.h"
#include "player.h"

#include <math.h>
#include <stdlib.h>

double startViewX = 0.0;
double startViewY = 0.0;
bool startViewInitialized = false;

double lastViewXPosition = 0.0;
double lastViewYPosition = 0.0;

double currentYaw = 0.0;
double currentPitch = 0.0;

const double MAX_PITCH_RADIANS = (PI / 2.0) - 0.00017453292519943295;

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

void processMouseButtonActions(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        PlayerState ps = getPlayerState();

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (ps.isLookingAtBlock && ps.lookingAtBlock != NULL) {
                destroyBlock(getWorldStateGlobal(), (int)ps.lookingAtBlock->x, (int)ps.lookingAtBlock->y, (int)ps.lookingAtBlock->z);
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (ps.isLookingAtBlock && ps.lookingAtBlock != NULL) {
                GameElement* targetBlock = getBlockAtGlobal(getWorldStateGlobal(), (int)ps.lookingAtBlock->x, (int)ps.lookingAtBlock->y, (int)ps.lookingAtBlock->z);
                if (targetBlock == NULL || targetBlock->elementType == 0) {
                    return; // Cannot place on an air block or non-solid block
                }

                int face = getBlockFace();
                if (face == BLOCK_FACE_NONE) {
                    return;
                }

                int targetX = (int)ps.lookingAtBlock->x;
                int targetY = (int)ps.lookingAtBlock->y;
                int targetZ = (int)ps.lookingAtBlock->z;

                int newBlockX = targetX;
                int newBlockY = targetY;
                int newBlockZ = targetZ;

                switch (face) {
                    case BLOCK_FACE_TOP:    newBlockY = targetY + 1; break;
                    case BLOCK_FACE_BOTTOM: newBlockY = targetY - 1; break;
                    case BLOCK_FACE_FRONT:  newBlockZ = targetZ + 1; break;
                    case BLOCK_FACE_BACK:   newBlockZ = targetZ - 1; break;
                    case BLOCK_FACE_RIGHT:  newBlockX = targetX + 1; break;
                    case BLOCK_FACE_LEFT:   newBlockX = targetX - 1; break;
                    default: return; // Should not happen if face is valid and not BLOCK_FACE_NONE
                }

                int newBlockType = 1; // Place a default block type (e.g., stone)
                placeBlock(getWorldStateGlobal(), newBlockX, newBlockY, newBlockZ, newBlockType);
            }
        }
    }
}

void processMouseMoveActions(GLFWwindow* window, double x, double y) {
    if (!startViewInitialized) {
        startViewX = x;
        startViewY = y;
        lastViewXPosition = 0.0;
        lastViewYPosition = 0.0;
        startViewInitialized = true;
    }

    double relativeX = x - startViewX;
    double relativeY = y - startViewY;

    double viewDeltaX = relativeX - lastViewXPosition;
    double viewDeltaY = relativeY - lastViewYPosition;

    lastViewXPosition = relativeX;
    lastViewYPosition = relativeY;

    currentYaw += viewDeltaX / ROTATION_FACTOR;
    currentYaw = atan2(sin(currentYaw), cos(currentYaw));

    currentPitch -= viewDeltaY / ROTATION_FACTOR;
    currentPitch = fmax(-MAX_PITCH_RADIANS, fmin(currentPitch, MAX_PITCH_RADIANS));

    double longitude = currentYaw;
    double latitude = currentPitch;

    Vector3 rotation;

    rotation.x = cos(latitude) * cos(longitude);
    rotation.y = sin(latitude);
    rotation.z = cos(latitude) * sin(longitude);

    setViewportRotation(rotation);
}
