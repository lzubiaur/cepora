cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(libimgui C CXX)
set(IMGUI_SRC
  imgui.cpp
  imgui_demo.cpp
  imgui_draw.cpp
  examples/opengl3_example/imgui_impl_glfw_gl3.cpp
)
include_directories("../glfw/include" "../gl3w/include")
# Build a statc library which will be linked with a duktape module
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
  ${PROJECT_SOURCE_DIR}
  "${PROJECT_SOURCE_DIR}/examples/opengl3_example"
)
