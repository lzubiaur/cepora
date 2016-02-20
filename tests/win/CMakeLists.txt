# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.3)

#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /FORCE")

add_executable(test_mod main.c)
target_link_libraries(test_mod bar)

add_library(foo SHARED foo.c)
target_compile_definitions(foo PUBLIC BUILD_DLL=1)
target_link_libraries(foo bar)

add_library(bar SHARED bar.c)
target_compile_definitions(bar PUBLIC BUILD_DLL=1)

install(TARGETS test_mod foo bar
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/tests/win COMPONENT Runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/tests/win COMPONENT Runtime
)

