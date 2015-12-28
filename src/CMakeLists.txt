cmake_minimum_required(VERSION 3.3)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR})

add_executable(${RUNTIME_NAME} cpr_main.c cpr_error.c)
add_dependencies(${RUNTIME_NAME} js)
target_link_libraries(${RUNTIME_NAME} js)

install(TARGETS ${RUNTIME_NAME}
    BUNDLE  DESTINATION "." COMPONENT Runtime
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
    )
