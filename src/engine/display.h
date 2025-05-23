#ifndef BLOCKS_LOOP
#define BLOCKS_LOOP
#include <GLFW/glfw3.h>

void processDisplayLoop(GLFWwindow* window);

void renderText(float x, float y, char *string, float r, float g, float b);
void setupOrthographicProjection(int windowWidth, int windowHeight);
void restorePerspectiveProjection(void);

#endif