cmake_minimum_required(VERSION 3.3)
project(lib)

# Add third-party libraries
add_subdirectory(duktape)
add_subdirectory(glfw)

set(GLFW_HEADERS "${PROJECT_SOURCE_DIR}/glfw/include" PARENT_SCOPE)
