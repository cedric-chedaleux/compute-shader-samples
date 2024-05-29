cmake_minimum_required(VERSION 3.13)

SET(CMAKE_INSTALL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/install)
SET(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_ROOT})

# Build GL3W from sources since it is not available at Ubuntu package 
find_package(PythonInterp REQUIRED)
include(FetchContent)
FetchContent_Declare(gl3w GIT_REPOSITORY "https://github.com/skaslev/gl3w.git" GIT_TAG "master")
FetchContent_Populate(gl3w)

# GL3W builds the source and header from gl3w_gen.py script
execute_process(COMMAND ${PYTHON_EXECUTABLE} gl3w_gen.py WORKING_DIRECTORY ${gl3w_SOURCE_DIR} OUTPUT_QUIET)
add_library(gl3w INTERFACE)
target_sources(gl3w INTERFACE
  $<BUILD_INTERFACE:${gl3w_SOURCE_DIR}/src/gl3w.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_DATAROOTDIR}/gl3w/src/gl3w.c>)
target_sources(gl3w INTERFACE FILE_SET HEADERS
  BASE_DIRS
  ${gl3w_source_dir}/include
  FILES
  ${gl3w_headers})

find_package(OpenGL REQUIRED)

target_include_directories(gl3w INTERFACE
  $<BUILD_INTERFACE:${gl3w_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/gl3w>)
target_link_libraries(gl3w INTERFACE OpenGL::GL ${CMAKE_DL_LIBS})

add_subdirectory(ssbo_sample)
add_subdirectory(img_generation)
add_subdirectory(convert2gray)
add_subdirectory(boxblur)


install(DIRECTORY shaders/ DESTINATION shaders)