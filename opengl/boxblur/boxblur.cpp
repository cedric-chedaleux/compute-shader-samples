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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


int main()
{
    if (!initGL())
    {
        fprintf(stderr, "Failed to initialize GL!\n");
        return 1;
    }

    GLTime computeTime;
    printGLInfo();

    // Compile the compute shader and get its handle
    GLuint computeHandle = createComputeShader("boxblur.comp");

    // Square image with power of two size
    int w;
    int h;
    int numChannels = 4;

    // Load an image to a texture
    std::string inputFilePath = getBinDirectory() + "landscape.jpg";
    int inputNumChannels;
    auto input = stbi_load(inputFilePath.c_str(), &w, &h, &inputNumChannels, numChannels); // Force image to load with 4 channels
    if (!input) {
        fprintf(stderr, "Failed to load '%s'\n", inputFilePath.c_str());
        exit(39);
    }
    printf("Image loaded (width = %i, height = %i, number_channels = %i)\n", w, h, numChannels);
    GLuint inTex = createTextureStorage(0, GL_READ_ONLY, w, h, input);

    // Create the texture that will host the Black and white image
    GLuint outTex = createTextureStorage(1, GL_WRITE_ONLY, w, h); // Compute shader only write to RGBA format texture

    // Execute the compute shader in 16x16-size workground
    computeTime.start();
    glUseProgram(computeHandle);
    int localSize = 16;
    glDispatchCompute((w + localSize - 1) / localSize , (h + localSize - 1) / localSize, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    computeTime.end();
    
    // Buffer with 4 1-byte channels to store the texture data
    auto img = readTextureStorage(outTex, numChannels, w, h);
    const char* imgfile = "blur.png";
    stbi_write_png(imgfile, w, h, numChannels /* bytes per pixel */, img.data(), w * numChannels);
    printf("Image saved to '%s'\n", imgfile);
    
    // Print timestamp
    printf("\n");
    printf("========== Time execution ================\n");
    printf("Compute execution = %f ms\n", computeTime.timeInMs());
    printf("==========================================\n");

    closeGL();

    return 0;
}