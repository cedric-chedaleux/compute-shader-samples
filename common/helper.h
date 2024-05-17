// Software Name : compute_shader_samples
// SPDX-FileCopyrightText: Copyright (c) 2024 Cédric CHEDALEUX
// SPDX-License-Identifier: MIT
//
// This software is distributed under the MIT License;
// see the LICENSE file for more details.
//
// Author: Cédric CHEDALEUX <cedric.chedaleux@orange.com> et al

#pragma once

#include <fstream>
#include <string>
#include <limits.h>
#ifdef __linux__
#include <unistd.h>
#include <libgen.h>
#endif

bool loadFile(const std::string &filename, std::string &data)
{
    std::ifstream ifs(filename, std::ifstream::in);
    if (ifs.good())
    {
        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        data = std::move(content);
        return true;
    }
    else
    {
        return false;
    }
}

std::string parentDirectory(const std::string &str)
{
    const char kPathSeparator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif
    const std::string::size_type pos = str.find_last_of("/\\");
    // If no separator, return empty path.
    if (pos == std::string::npos)
    {
        return str + kPathSeparator + "..";
    }
    // If the separator is not trailing, we are done.
    if (pos < str.size() - 1)
    {
        return str.substr(0, pos);
    }
    // Else we have to look for the previous one.
    const std::string::size_type pos1 = str.find_last_of("/\\", pos - 1);
    return str.substr(0, pos1);
}

std::string getInstallDirectory()
{
    char exePath[4095];

#ifdef _WIN32
    unsigned int len = GetModuleFileNameA(GetModuleHandleA(0x0), exePath, MAX_PATH);
    std::string installDirectory = parentDirectory(parentDirectory(exePath));
#elif __linux__
    unsigned int len = 0;

    char result[PATH_MAX];
    ssize_t c = readlink("/proc/self/exe", result, PATH_MAX);
    len = c;
    result[len] = '\0';
    const char *path;
    if (c != -1)
        path = dirname(result);
    else
        fprintf(stderr, "Cant find executable path");

    std::string installDirectory(parentDirectory(path));
#endif

    if (len == 0)
    {
        fprintf(stderr, "Cant find executable path");
    }
    return installDirectory;
}

std::string getGLSLShaderDirectory() {
#ifdef __linux__
    return getInstallDirectory() + "/shaders/glsl/";
#elif _WIN32
    return getInstallDirectory() + "\\shaders\\glsl\\";
#else
    return "";
#endif
}

std::string getBinDirectory() {
#ifdef __linux__
    return getInstallDirectory() + "/bin/";
#elif _WIN32
    return getInstallDirectory() + "\\bin\\";
#else
    return "";
#endif
}