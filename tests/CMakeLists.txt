# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.3)

configure_file("run-tests.sh.in" "${PROJECT_BINARY_DIR}/run-tests.sh" @ONLY)

install(
  FILES test.coffee module.coffee glfw.coffee broken.coffee
  DESTINATION ${BUNDLE_RESOURCE_DIR}/js/tests
  COMPONENT Runtime)

install(
  PROGRAMS "${PROJECT_BINARY_DIR}/run-tests.sh"
  DESTINATION ${CMAKE_INSTALL_PREFIX})
