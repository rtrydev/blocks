#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include "engine/window.h"
#include "engine/display.h"
#include "engine/cube.h"
#include "engine/framebuffer.h" // Added
#include "engine/shader.h"     // Added
#include "engine/world.h"      // Added (for drawWorldDepth)
#include "engine/frustum.h"    // Added (for Frustum struct, used by drawWorldDepth)
#include "engine/player.h"     // Added (for getPlayerState to update view matrix)
#include "engine/viewport.h"   // Added (for getViewport, and potentially setViewport if needed)

// Depth map dimensions (can be configured)
#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

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

        // Initialize VBOs for cubes
        initCubeVBOs();

        // Load depth shader
        GLuint depthShaderProgram = loadShaders("shaders/depth_vertex.glsl", "shaders/depth_fragment.glsl");
        if (depthShaderProgram == 0) {
            fprintf(stderr, "Failed to load depth shaders.\n");
            freeCubeVBOs();
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        // Create Depth Map FBO
        DepthMapFBO depthMapFBO;
        if (!createDepthMapFBO(&depthMapFBO, SHADOW_WIDTH, SHADOW_HEIGHT)) {
            fprintf(stderr, "Failed to create depth map FBO.\n");
            glDeleteProgram(depthShaderProgram);
            freeCubeVBOs();
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        // The processDisplayLoop now needs to be aware of the depth pass.
        // For this example, I'll extract the core rendering logic from processDisplayLoop
        // and put it here, or modify processDisplayLoop to accept the depth shader and FBO.
        // For simplicity, let's assume processDisplayLoop is where the main rendering happens
        // and we are modifying it or the functions it calls.
        // The following is a conceptual representation of how the loop would be modified.
        // The actual `processDisplayLoop` in `display.c` would need to be refactored.

        // --- Main Loop (Conceptual - actual loop is in processDisplayLoop) ---
        // while (!glfwWindowShouldClose(window)) {
        //     // --- Depth Pass ---
        //     glUseProgram(depthShaderProgram);
        //     bindDepthMapFBO(&depthMapFBO); // Binds FBO and sets viewport
        //     glClear(GL_DEPTH_BUFFER_BIT);
        //     glEnable(GL_DEPTH_TEST);

        //     // Set up view and projection matrices for the light's perspective (example)
        //     // For now, we'll use the player's perspective for simplicity of drawWorldDepth
        //     PlayerState player = getPlayerState();
        //     Frustum frustum = getFrustum(); // Assuming getFrustum uses current player state
        //     // updateFrustum(&frustum, player.position, player.horizontalAngle, player.verticalAngle); // Update if needed

        //     // Get view and projection matrices (these would be passed to the shader)
        //     // This part is highly dependent on how matrices are handled in your engine.
        //     // The depth shader expects model, view, projection uniforms.
        //     // For now, drawWorldDepth and drawCubeDepth use fixed-function pipeline matrix operations
        //     // (glPushMatrix, glTranslatef, etc.) which won't directly work with a shader expecting uniforms.
        //     // This is a major point of refactoring needed if this were a modern OpenGL app.
        //     // Given the existing codebase uses glColorPointer, glVertexPointer, glPushMatrix,
        //     // it's using legacy OpenGL. Shaders and FBOs are newer features.
        //     // For the depth pass to TRULY use the depth shaders, drawWorldDepth/drawCubeDepth
        //     // would need to be rewritten to not use fixed-function pipeline for transformations
        //     // and instead accept matrices to pass to the shader.
        //     // However, the task description implies using the existing draw functions as a base.
        //     // The depth shaders provided WILL NOT be correctly used by the current drawCubeDepth
        //     // as it uses glVertexPointer and glTranslate rather than shader attributes and uniforms.
        //     // This is a fundamental conflict.
        //
        //     // For the purpose of this exercise, we'll call drawWorldDepth,
        //     // acknowledging that it won't correctly use the depthShaderProgram without major refactoring
        //     // of the drawing functions to be shader-based.
        //     // The FBO will capture depth from the fixed-function pipeline rendering.
        //     drawWorldDepth(&frustum); // Pass the frustum

        //     unbindDepthMapFBO();
        //     glUseProgram(0); // Unbind shader program

        //     // --- Main Render Pass (Simplified) ---
        //     // Reset viewport to main window size (assuming getViewport gives main window dimensions)
        //     Viewport mainViewport = getViewport(); // You might need to get this from window size
        //     glViewport(0, 0, mainViewport.width, mainViewport.height);
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //     // ... (rest of the normal rendering using default FBO, other shaders/fixed function)
        //     // drawWorld(&frustum); // Example: regular drawing
        //     // ...
        //
        //     // glfwSwapBuffers(window);
        //     // glfwPollEvents();
        // }
        // --- End Conceptual Main Loop ---

        // Pass necessary new objects to processDisplayLoop or integrate logic directly
        // For now, let's assume processDisplayLoop is modified to handle this internally
        // or that we are showing the conceptual structure.
        // The original call was: processDisplayLoop(window);
        // A modified call might look like:
        // processDisplayLoop(window, depthShaderProgram, &depthMapFBO);
        // For now, I will call the original processDisplayLoop and assume the integration
        // happens within it or the functions it calls, as per the subtask's focus on creating
        // the depth pass components. The full integration into the display loop
        // is a larger refactoring task.

        // processDisplayLoop(window); // This function will need internal modification
        processDisplayLoop(window, depthShaderProgram, &depthMapFBO); // MODIFIED: Pass shader and FBO

        // Cleanup
        deleteDepthMapFBO(&depthMapFBO);
        glDeleteProgram(depthShaderProgram);
        freeCubeVBOs();

        glfwDestroyWindow(window);
    }
    else {
        fprintf(stderr, "initWindow failed, terminating.\n");
    }

    glfwTerminate();
    return 0;
}