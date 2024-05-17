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


int main()
{
    if (!initGL())
    {
        fprintf(stderr, "Failed to initialize GL!\n");
        return 1;
    }

    printGLInfo();

    // Compile the compute shader and get its handle
    GLuint computeHandle = createComputeShader("img_generation.comp");

    // Square image with power of two size
    int w = 512;
    int h = 512;
    int numChannels = 4; // RGBA

    // Create the texture that will host the image
    GLuint outTex;
    glGenTextures(1, &outTex);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8UI, w, h); // Each pixel will be stored in 4 unsigned integer [0,255]
    glActiveTexture(GL_TEXTURE0);
    glBindImageTexture(0, outTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI);

    // Execute the compute shader in 32x32-size workground
    glUseProgram(computeHandle);
    GLint location = glGetUniformLocation(computeHandle, "texture");
    glUniform1i(location, 0);
    glDispatchCompute(w / 32 , h / 32, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    
    // Buffer with 4 1-byte channels to store the texture data
    std::vector<uint8_t> img(w * h * numChannels, 1.0);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, img.data());
    const char* imgfile = "image.png";
    stbi_write_png(imgfile, w, h, numChannels /* bytes per pixel */, img.data(), w * numChannels);
    printf("Image saved to '%s'\n", imgfile);

    closeGL();

    return 0;
}