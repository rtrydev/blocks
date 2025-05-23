#ifndef BLOCKS_USERINPUTS
#define BLOCKS_USERINPUTS

#include <stdbool.h>
#include <GLFW/glfw3.h>

typedef struct InputState {
	bool UP_ACTIVE;
	bool DOWN_ACTIVE;
	bool RIGHT_ACTIVE;
	bool LEFT_ACTIVE;
	bool JUMP_ACTIVE;
} InputState;

InputState getInputState();

void processInputTick();
void processKeyboardButtonActions(GLFWwindow* window, int key, int scancode, int action, int mods);
void processMouseButtonActions(GLFWwindow* window, int button, int action, int mods);
void processMouseMoveActions(GLFWwindow* window, double x, double y);

#endif
