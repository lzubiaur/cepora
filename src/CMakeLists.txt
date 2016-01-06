cmake_minimum_required(VERSION 3.3)

configure_file("cpr_config.h.in" "${PROJECT_BINARY_DIR}/src/cpr_config.h")

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR}/lib/duktape)

add_executable(${RUNTIME_NAME} cpr_main.c cpr_error.c cpr_sys_tools.c cpr_mod_coffee.c cpr_loadlib.c)
add_dependencies(${RUNTIME_NAME} duktape)
target_link_libraries(${RUNTIME_NAME} duktape)

target_compile_options(${RUNTIME_NAME} PUBLIC -std=c99 -pedantic-errors -pedantic)

if (BUILD_OSX)
  target_compile_definitions(${RUNTIME_NAME} PRIVATE CPR_BUILD_OSX=1)
elseif (BUILD_LINUX)
  target_compile_definitions(${RUNTIME_NAME} PRIVATE CPR_BUILD_LINUX=1)
elseif (BUILD_WINDOWS)
  target_compile_definitions(${RUNTIME_NAME} PRIVATE CPR_BUILD_WINDOWS=1)
endif()

# TODO Mac OS X only
# Specify linker flags for shared library build
# link with the option '-undefined dynamic_lookup' to avoid getting undefined references
# (http://lua-users.org/wiki/BuildingModules)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
add_library(io SHARED cpr_io.c)
set_target_properties(io PROPERTIES PREFIX "" OUTPUT_NAME "io" SUFFIX ".so")
# add_dependencies(io duktape_shared)
# target_link_libraries(io duktape_shared)

install(TARGETS ${RUNTIME_NAME} io
    BUNDLE  DESTINATION "." COMPONENT Runtime
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    )
