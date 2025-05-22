#ifndef BLOCKS_LOOP
#define BLOCKS_LOOP
#include <GLFW/glfw3.h>
#include "framebuffer.h" // Added for DepthMapFBO
#include <GL/glew.h>     // Added for GLuint (shader program ID)

void processDisplayLoop(GLFWwindow* window, GLuint depthShaderProgram, const DepthMapFBO* depthMapFBO);

// UI Rendering Functions
void renderText(float x, float y, char *string, float r, float g, float b);
void setupOrthographicProjection(int windowWidth, int windowHeight);
void restorePerspectiveProjection(void);

#endif