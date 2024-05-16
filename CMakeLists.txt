cmake_minimum_required(VERSION 3.13)

SET(CMAKE_INSTALL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/install)
SET(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_ROOT})

add_subdirectory(ssbo_sample)

install(DIRECTORY shaders/ DESTINATION shaders)