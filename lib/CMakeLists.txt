cmake_minimum_required(VERSION 3.3)
project(lib)

# Add third-party libraries
add_subdirectory(gl3w)
add_subdirectory(glfw)
add_subdirectory(duktape)
add_subdirectory(imgui)

set(GLFW_HEADERS "${PROJECT_SOURCE_DIR}/glfw/include" PARENT_SCOPE)
