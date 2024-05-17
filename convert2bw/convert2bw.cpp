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


GLuint createTextureStorage(GLuint unit, GLenum access, int width, int height, unsigned char* data = nullptr) {
    GLuint tex;
    GLenum internalFormat = GL_RGBA8UI; // Each pixel will be stored in 4 unsigned integer [0,255]
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
    if (data) {
        glTexSubImage2D(GL_TEXTURE_2D, 0 /* mipmap level */, 0,0, width, height, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindImageTexture(unit, tex, 0, GL_FALSE, 0, access, internalFormat);
    return tex;
}

std::vector<uint8_t> readTextureStorage(GLuint tex, int numChannels, int width, int height) {
    std::vector<uint8_t> img(width * height * numChannels);
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, img.data());
    return img;
}

int main()
{
    if (!initGL())
    {
        fprintf(stderr, "Failed to initialize GL!\n");
        return 1;
    }

    printGLInfo();

    // Compile the compute shader and get its handle
    GLuint computeHandle = createComputeShader("convert2bw.comp");

    // Square image with power of two size
    int w;
    int h;
    int numChannels = 4;

    // Load an image to a texture
    std::string inputFilePath = getBinDirectory() + "Lenna.png";
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

    // Execute the compute shader in 32x32-size workground
    glUseProgram(computeHandle);
    glDispatchCompute(w / 32 , h / 32, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    
    // Buffer with 4 1-byte channels to store the texture data
    auto img = readTextureStorage(outTex, numChannels, w, h);
    const char* imgfile = "bw.png";
    stbi_write_png(imgfile, w, h, numChannels /* bytes per pixel */, img.data(), w * numChannels);
    printf("Image saved to '%s'\n", imgfile);

    closeGL();

    return 0;
}