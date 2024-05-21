cmake_minimum_required(VERSION 3.13)
project(boxblur)

include_directories("../common")
add_executable(${PROJECT_NAME}
  boxblur.cpp
)

find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE gl3w OpenGL::GL)
target_link_libraries(${PROJECT_NAME} PRIVATE gl3w OpenGL::GL)

# GLFW3 for window abstraction layer
if(WIN32)
find_package(GLFW3 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
else()
find_package(PkgConfig)
PKG_CHECK_MODULES(GLFW3 REQUIRED glfw3)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW3_LIBRARIES})
endif()

# Install
install(TARGETS ${PROJECT_NAME})
install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> TYPE BIN)
install(FILES landscape.jpg DESTINATION bin)
