project(duktape C)
cmake_minimum_required(VERSION 3.3)

include_directories(${PROJECT_SOURCE_DIR})
add_library(js STATIC duktape.c)
target_include_directories(js PUBLIC ${PROJECT_SOURCE_DIR})
