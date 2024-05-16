// Software Name : compute_shader_samples
// SPDX-FileCopyrightText: Copyright (c) 2024 Cédric CHEDALEUX
// SPDX-License-Identifier: MIT
//
// This software is distributed under the MIT License;
// see the LICENSE file for more details.
//
// Author: Cédric CHEDALEUX <cedric.chedaleux@orange.com> et al

#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#elif _WIN32
#define GLFW_EXPOSE_NATIVE_WIN3
#endif


#ifndef NDEBUG
// If there is a glError this outputs it along with a message to stderr.
// otherwise there is no output.
void GLErrorCheck(const char *message)
{
    GLenum val = glGetError();
    switch (val)
    {
    case GL_INVALID_ENUM:
        fprintf(stderr, "GL_INVALID_ENUM : %s\n", message);
        break;
    case GL_INVALID_VALUE:
        fprintf(stderr, "GL_INVALID_VALUE : %s\n", message);
        break;
    case GL_INVALID_OPERATION:
        fprintf(stderr, "GL_INVALID_OPERATION : %s\n", message);
        break;
#ifndef GL_ES_VERSION_2_0
    case GL_STACK_OVERFLOW:
        fprintf(stderr, "GL_STACK_OVERFLOW : %s\n", message);
        break;
    case GL_STACK_UNDERFLOW:
        fprintf(stderr, "GL_STACK_UNDERFLOW : %s\n", message);
        break;
#endif
    case GL_OUT_OF_MEMORY:
        fprintf(stderr, "GL_OUT_OF_MEMORY : %s\n", message);
        break;
    case GL_NO_ERROR:
        break;
    }
}
#endif

GLuint createComputeShader(const std::string& filename)
{
    // Creating the compute shader, and the program object containing the shader
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

    std::string csSrc;
    std::string shaderFilePath = getGLSLShaderDirectory() + filename;
    if (!loadFile(shaderFilePath, csSrc))
    {
        fprintf(stderr, "Error in loading the compute shader %s\n", shaderFilePath.c_str());
        exit(39);
    }

    const GLchar *sourcePtr = csSrc.c_str();
    int size = static_cast<int>(csSrc.size());
    glShaderSource(cs, 1, (const GLchar **)&sourcePtr, &size);
    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue)
    {
        fprintf(stderr, "Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(cs, 10239, &length, log);
        fprintf(stderr, "Compiler log:\n%s\n", log);
        exit(40);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue)
    {
        fprintf(stderr, "Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(progHandle, 10239, &length, log);
        fprintf(stderr, "Linker log:\n%s\n", log);
        exit(41);
    }

    GLErrorCheck("Compute shader");
    return progHandle;
}

GLFWwindow *offscreen_context = nullptr;

// Create an OpenGL context
bool initGL()
{
    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    offscreen_context = glfwCreateWindow(250, 250, "test", NULL, NULL);
    if (!offscreen_context)
    {
        return false;
    }
    glfwMakeContextCurrent(offscreen_context);
    glfwSwapInterval(0);

    if (gl3wInit())
    {
        return false;
    }
    return true;
}

void closeGL()
{
    glfwDestroyWindow(offscreen_context);
    glfwTerminate();
}
