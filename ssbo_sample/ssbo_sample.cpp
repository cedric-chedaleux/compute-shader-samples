// Software Name : compute_shader_samples
// SPDX-FileCopyrightText: Copyright (c) 2024 Cédric CHEDALEUX
// SPDX-License-Identifier: MIT
//
// This software is distributed under the MIT License;
// see the LICENSE file for more details.
//
// Author: Cédric CHEDALEUX <cedric.chedaleux@orange.com> et al

#ifdef _WIN32
// #pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "OpenGL32.Lib")
#include <windows.h>
#endif

#include <GL/gl3w.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iterator>
#include <numeric>
#include <chrono>

#include "helper.h"
#include "gl_helper.h"

// Create an vector of successive value from 1 to 'count'
std::vector<int> createSuccessiveVector(size_t count)
{
    std::vector<int> arr(count);
    std::iota(arr.begin(), arr.end(), 1);
    return arr;
}

void printArrays(const char *msg, const std::vector<int> &arr)
{
    printf("%s: ", msg);
    if (arr.size() <= 8)
    {
        for (auto &i : arr)
            printf("%i,", i);
    }
    else
    {
        auto bIt = arr.cbegin();
        auto b4It = std::next(arr.cbegin(), 4);
        for (auto it = bIt; it != b4It; ++it)
            printf("%i,", *it);
        printf("...");
        auto eIt = arr.cend();
        auto e4It = std::prev(arr.cend(), 4);
        for (auto it = e4It; it != eIt; ++it)
            printf("%i,", *it);
    }
    printf("\n");
}

GLuint createSSBO(const std::vector<int> &data, int index /*binding index in shader*/)
{
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), data.data(), GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    GLErrorCheck("SSBO creation");
    return ssbo;
}

void readSSBO(GLuint ssbo, std::vector<int> &outputs)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * outputs.size(), outputs.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
}

int main()
{
    if (!initGL())
    {
        fprintf(stderr, "Failed to initialize GL!\n");
        return 1;
    }

    printGLInfo();
    GLTime computeTime;

    // Compile the compute shader and get its handle
    GLuint computeHandle = createComputeShader("ssbo_sample.comp");

    // Create input data
    int nbIntegers = 8;
    //int nbIntegers = 1 << 24; // 16 millions of integers
    auto inputs = createSuccessiveVector(nbIntegers);
    printArrays("inputs", inputs);

    auto tStart = std::chrono::high_resolution_clock::now();

    // Create two shader storage objects (one for input and one for output)
    GLuint inputSSBO = createSSBO(inputs, 0);
    GLuint outputSSBO = createSSBO(std::vector<int>(inputs.size(), 0), 1);

    // Execute the compute shader
    computeTime.start();
    glUseProgram(computeHandle);
    glDispatchCompute(nbIntegers, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    computeTime.end();

    // Read result back to CPU
    auto outputs = std::vector<int>(inputs.size());
    readSSBO(outputSSBO, outputs);
    auto tEnd = std::chrono::high_resolution_clock::now();
    printArrays("outputs", outputs);

    // Print timestamp
    printf("\n");
    printf("========== Time execution ================\n");
    printf("Compute execution = %f ms\n", computeTime.timeInMs());
    printf("Total execution   = %f ms\n", std::chrono::duration<double, std::milli>(tEnd - tStart).count());
    printf("==========================================\n");

    closeGL();

    return 0;
}