set(PROJECT_NAME "cepora")
set(RUNTIME_NAME ${PROJECT_NAME})

project(${PROJECT_NAME} C)
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

# TODO GLFW compile options http://www.glfw.org/docs/latest/compile.html#compile_options_osx

execute_process(COMMAND git describe --always --dirty OUTPUT_VARIABLE CPR_GIT_DESCRIBE)
execute_process(COMMAND git rev-parse HEAD OUTPUT_VARIABLE CPR_GIT_COMMIT)
# Remove new line characters
string(REPLACE "\n" "" CPR_GIT_DESCRIBE ${CPR_GIT_DESCRIBE})
string(REPLACE "\n" "" CPR_GIT_COMMIT   ${CPR_GIT_COMMIT})
# Read version from cpr_main.c
file(STRINGS ${PROJECT_SOURCE_DIR}/src/cpr_main.c CPR_VERSION_STRING REGEX "#define CPR_VERSION_STRING.*")
# Extract version string
string(REGEX MATCHALL "[0-9]+" CPR_VERSION_STRING ${CPR_VERSION_STRING})
# Get major, minor and patch version
list(GET CPR_VERSION_STRING 0 CPR_VERSION_MAJOR)
list(GET CPR_VERSION_STRING 1 CPR_VERSION_MINOR)
list(GET CPR_VERSION_STRING 2 CPR_VERSION_PATCH)

message(STATUS "Building Cepora version ${CPR_VERSION_MAJOR}.${CPR_VERSION_MINOR}.${CPR_VERSION_PATCH}")
message(STATUS "Git describe ${CPR_GIT_DESCRIBE}")
message(STATUS "Git commit ${CPR_GIT_COMMIT}")

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/products/${CMAKE_BUILD_TYPE})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# add_definitions(-Wall -std=c++0x)
add_definitions(-Wall)

# Include the dependencies, tests and source
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)

install(DIRECTORY js DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime)
