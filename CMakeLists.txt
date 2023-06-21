cmake_minimum_required(VERSION 3.22.1)

project(
  OpenGL-Playground
  VERSION 1.0
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# GLFW
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_INSTALL OFF)

add_subdirectory(external/glfw)

target_include_directories(
  glfw
  PUBLIC
  external/glfw/include
)

# GLAD
add_library(
  glad
  STATIC
  external/glad/src/glad.c
)

target_include_directories(
  glad
  PUBLIC
  external/glad/include
)

add_subdirectory(src)