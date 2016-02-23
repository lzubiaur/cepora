project(duktape C)
cmake_minimum_required(VERSION 3.3)

include_directories(${PROJECT_SOURCE_DIR})
add_library(duktape SHARED duktape.c)
target_include_directories(duktape PUBLIC ${PROJECT_SOURCE_DIR})

# http://duktape.org/guide.html#compiling
if(BUILD_WIN)
  # Add -DDUK_OPT_DLL_BUILD when building shared library on Windows
  target_compile_definitions(duktape PRIVATE DUK_OPT_DLL_BUILD=1)
  # On Windows we must install DLL dependencies manually
  install(TARGETS duktape
    RUNTIME DESTINATION ${BUNDLE_RUNTIME_DESTINATION} COMPONENT Runtime
    LIBRARY DESTINATION ${BUNDLE_RUNTIME_DESTINATION} COMPONENT Runtime
  )
else()
  # GCC/Clang flags
  target_compile_options(duktape PRIVATE -std=c99 -Os -fomit-frame-pointer -fstrict-aliasing)
endif()

# target_compile_definitions(duktape PRIVATE DUK_OPT_DEBUGGER_SUPPORT=1 DUK_OPT_INTERRUPT_COUNTER=1 DUK_CMDLINE_DEBUGGER_SUPPORT=1)
# Enable Duktape debug logs
# target_compile_definitions(duktape PRIVATE DUK_OPT_DEBUG=1 DUK_OPT_DPRINT=1 DUK_OPT_DPRINT_COLORS=1)
