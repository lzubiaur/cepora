cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

include(config.cmake)

set(PROJECT_NAME ${CPR_PROJECT_NAME})
set(RUNTIME_NAME ${PROJECT_NAME})
set(BUNDLE_NAME  ${PROJECT_NAME})

project(${PROJECT_NAME} C CXX)

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

# Add custom cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(BUILD_OSX)
  set(BUNDLE_NAME "${BUNDLE_NAME}.app")
  set(BUNDLE_RESOURCE_DIR "${BUNDLE_NAME}/Contents/Resources")
  set(PLUGINS_DESTINATION "${CMAKE_INSTALL_PREFIX}/${BUNDLE_NAME}/Contents/MacOS")
  set(BUNDLE_RUNTIME_DESTINATION "${BUNDLE_NAME}/Contents/MacOS")
  # Disable RPATH for build and install. We use fixup_bundle to assemble a
  # standalone bundle.
  # CMAKE_SKIP_RPATH is required disabled on OSX because dependencies like
  # GLFW are linked using the install path and fixup_bundle can't find them if they
  # are not yet installed (which we don't want to do).
  set(CMAKE_SKIP_RPATH TRUE)
elseif(BUILD_LINUX)
  set(BUNDLE_RESOURCE_DIR "${BUNDLE_NAME}")
  set(PLUGINS_DESTINATION "${CMAKE_INSTALL_PREFIX}/${BUNDLE_NAME}/bin")
  set(BUNDLE_RUNTIME_DESTINATION "${BUNDLE_NAME}/bin")

  # use, i.e. don't skip the full RPATH for the build tree
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

  # the RPATH to be used when installing
  SET(CMAKE_INSTALL_RPATH ".")

  # don't add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
elseif(BUILD_WIN)
  set(BUNDLE_RESOURCE_DIR "${BUNDLE_NAME}")
  set(PLUGINS_DESTINATION "${CMAKE_INSTALL_PREFIX}/${BUNDLE_NAME}/bin")
  set(BUNDLE_RUNTIME_DESTINATION "${BUNDLE_NAME}/bin")
endif(BUILD_OSX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
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

install(DIRECTORY js DESTINATION ${BUNDLE_RESOURCE_DIR} COMPONENT Runtime)
