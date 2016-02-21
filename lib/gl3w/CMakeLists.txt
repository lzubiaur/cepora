cmake_minimum_required(VERSION 2.8)

project(libgl3w)
include_directories(include)
add_library(gl3w STATIC src/gl3w.c)
target_include_directories(gl3w PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(BUILD_LINUX)
  # dl (libdl.so) for dynamic loaded library (dlopen, dlsym...)
  # Note that gl3w has been hacked (gl3w_gen.py) so there's no need
  # to link against libGL (required because of glXGetProcAddress)
  target_link_libraries(gl3w dl)
  # Since this static library will be linked against a shared library (gl3w module)
  # it has to be build with the PIC flag (position-independent code)
  target_compile_options(gl3w PUBLIC -fPIC)
elseif(BUILD_OSX)
  # Enable objective-c (doesn't seem to be required though)
  target_compile_options(gl3w PRIVATE -x objective-c)
  # Setting "-x objective-c" with CMAKE_C_FLAGS doesn't work when building with
  # Makefile generator (though it works when building using the XCode generator)
  # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -x objective-c")
  # Linking against either Cocoa, Fondation or AppKit framework is required
  target_link_libraries(gl3w
  # "-framework Cocoa"
  # "-framework AppKit"
  "-framework Foundation"
  )
endif()
