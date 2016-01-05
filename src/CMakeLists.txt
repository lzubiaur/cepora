cmake_minimum_required(VERSION 3.3)

configure_file("cpr_config.h.in" "${PROJECT_BINARY_DIR}/src/cpr_config.h")

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR}/lib/duktape)

add_executable(${RUNTIME_NAME} cpr_main.c cpr_error.c cpr_sys_tools.c cpr_mod_coffee.c)
add_dependencies(${RUNTIME_NAME} duktape)

if (BUILD_LINUX)
  target_link_libraries(${RUNTIME_NAME} duktape dl m)
  target_compile_options(${RUNTIME_NAME} PUBLIC -std=c99)
endif()

# Enable debugger support
# target_compile_definitions(${RUNTIME_NAME} PRIVATE DUK_OPT_DEBUGGER_SUPPORT=1 DUK_OPT_INTERRUPT_COUNTER=1 DUK_CMDLINE_DEBUGGER_SUPPORT=1)

if (BUILD_MACOS)
# Specify linker flags for shared library build
# link with the option '-undefined dynamic_lookup' to avoid getting undefined references
# (http://lua-users.org/wiki/BuildingModules)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
endif (BUILD_MACOS)
add_library(io SHARED cpr_mod_io.c)
# add_dependencies(io duktape_shared)
# target_link_libraries(io duktape_shared)

install(TARGETS ${RUNTIME_NAME} io
    BUNDLE  DESTINATION "." COMPONENT Runtime
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    )
