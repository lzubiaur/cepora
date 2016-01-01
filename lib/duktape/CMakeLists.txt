project(duktape C)
cmake_minimum_required(VERSION 3.3)

include_directories(${PROJECT_SOURCE_DIR})
add_library(duktape STATIC duktape.c)
target_include_directories(duktape PUBLIC ${PROJECT_SOURCE_DIR})
