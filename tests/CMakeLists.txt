# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.3)

configure_file("run-tests.sh.in" "${PROJECT_BINARY_DIR}/run-tests.sh" @ONLY)

set(tests_files
  hello.coffee
  module.coffee
  loadlib.coffee
  glfw.coffee
  broken.coffee
  arguments.coffee
  gl3w.coffee
  imgui.coffee
)


if (BUILD_WIN)
  message(STATUS "Build Windows tests")
  add_subdirectory(win)
endif (BUILD_WIN)


install(
  FILES ${tests_files}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/tests
  COMPONENT Runtime)

install(
  PROGRAMS "${PROJECT_BINARY_DIR}/run-tests.sh" run-testcase.py
  DESTINATION ${CMAKE_INSTALL_PREFIX})
