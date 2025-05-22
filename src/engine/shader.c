#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to read shader file
char* readFile(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory for shader file: %s\n", filepath);
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

// Helper function to compile shader
GLuint compileShader(const char *source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        glDeleteShader(shader); // Don't leak the shader.
        return 0;
    }
    return shader;
}

GLuint loadShaders(const char *vertex_file_path, const char *fragment_file_path) {
    // 1. Retrieve the vertex/fragment source code from filePath
    char* vertexShaderSource = readFile(vertex_file_path);
    if (!vertexShaderSource) return 0;
    char* fragmentShaderSource = readFile(fragment_file_path);
    if (!fragmentShaderSource) {
        free(vertexShaderSource);
        return 0;
    }

    // 2. Compile shaders
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    free(vertexShaderSource); // Free memory once compiled
    if (!vertexShader) {
        free(fragmentShaderSource);
        return 0;
    }

    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    free(fragmentShaderSource); // Free memory once compiled
    if (!fragmentShader) {
        glDeleteShader(vertexShader); // Clean up vertex shader if fragment fails
        return 0;
    }

    // 3. Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        return 0;
    }

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
