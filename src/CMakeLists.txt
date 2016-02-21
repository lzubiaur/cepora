cmake_minimum_required(VERSION 3.3)

configure_file("cpr_config.h.in" "${PROJECT_BINARY_DIR}/src/cpr_config.h")

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${GLFW_HEADERS}
  ${PROJECT_SOURCE_DIR}/lib/duktape)

# Custom C flags
set(C_FLAGS -std=c99 -pedantic-errors -pedantic)

# Compilation definitions
set(CPR_COMPILE_DEF)
# Flag to notify that this build is part of the Cepora framework
list(APPEND CPR_COMPILE_DEF CPR_COMPILING_CEPORA=1)
# Note that internal debugging must be enable in both runtime and modules to use
# internal debugging in modules.
if (CPR_BUILD_DEBUG_INTERNAL)
  list(APPEND CPR_COMPILE_DEF CPR_DEBUG_INTERNAL=1)
endif(CPR_BUILD_DEBUG_INTERNAL)

# Set default symbols visibility to `hidden`
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Cepora core source files
set(CEPORA_SRC
  cpr_cepora.c
  cpr_error.c
  cpr_sys_tools.c
  cpr_loadlib.c
  cpr_package.c
  cpr_duktape_helpers.c
  cpr_debug_internal.c)

# Cepora shared library
add_library(cepora SHARED ${CEPORA_SRC})
add_dependencies(cepora duktape)
target_link_libraries(cepora duktape)
# Enable duktape debugger support
# target_compile_definitions(cepora PRIVATE DUK_OPT_DEBUGGER_SUPPORT=1 DUK_OPT_INTERRUPT_COUNTER=1 DUK_CMDLINE_DEBUGGER_SUPPORT=1)

# Runtime executable
add_executable(rt cpr_main.c)
set_target_properties(rt PROPERTIES OUTPUT_NAME ${RUNTIME_NAME})
add_dependencies(rt cepora)
target_link_libraries(rt cepora)

if (BUILD_LINUX)
  set(MODULE_SUFFIX ".so")
  target_link_libraries(cepora dl m)
  target_compile_options(cepora PUBLIC ${C_FLAGS})
  target_compile_options(rt PUBLIC ${C_FLAGS})
  list(APPEND CPR_COMPILE_DEF CPR_BUILD_LINUX=1)
  # On linux APP must be set to the executable binary
  set(APP "${CMAKE_INSTALL_PREFIX}/${BUNDLE_NAME}/bin/${RUNTIME_NAME}")
elseif (BUILD_OSX)
  set(MODULE_SUFFIX ".so")
  list(APPEND CPR_COMPILE_DEF CPR_BUILD_OSX=1)
  # On OSX APP must be set to the bundle root
  set(APP "${CMAKE_INSTALL_PREFIX}/${BUNDLE_NAME}")
  # MACOSX_BUNDLE can not be used with CPack
  set_target_properties(rt PROPERTIES MACOSX_BUNDLE true)
  # Specify linker flags for shared library build
  # -undefined : specify how undefined symbols are treated (default is an error).
  # Since we create shared libraries (modules) we use `dynamic_lookup` to avoid
  # getting undefined references errors to duktape API.
  # (http://lua-users.org/wiki/BuildingModules)
  # -flat_namespace : the linker does not record which dylib an external symbol
  # came from, so at runtime dyld again searches all images and uses the first definition it finds.
  # -bind_at_load : bind all symbols when the binary is loaded, rather than lazily
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
elseif (BUILD_WIN)
  set(MODULE_SUFFIX ".dll")
  # Shlwapi.lib : PathIsRelative, PathFileExists
  target_link_libraries(cepora Shlwapi)
  # CMAKE_C_VISIBILITY_PRESET
  # CMAKE_CXX_VISIBILITY_PRESET
  list(APPEND CPR_COMPILE_DEF CPR_BUILD_WINDOWS=1)
  # Linker options
  # -FORCE : Forces link to complete in spite of unresolved or multiply defined symbols
  # set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /FORCE")
  set_target_properties(${RUNTIME_NAME} PROPERTIES WIN32_EXECUTABLE true)
endif()

# Add defines to the runtime and cepora library
target_compile_definitions(cepora PUBLIC ${CPR_COMPILE_DEF})
target_compile_definitions(${RUNTIME_NAME} PUBLIC ${CPR_COMPILE_DEF})

################################################################################
# Modules
################################################################################

### DUMMY ######################################################################
add_library(mod_dummy SHARED modules/cpr_dummy.c)
set_target_properties(mod_dummy PROPERTIES PREFIX "" OUTPUT_NAME "dummy" SUFFIX "${MODULE_SUFFIX}")
target_compile_definitions(mod_dummy PRIVATE ${CPR_COMPILE_DEF})
if (BUILD_LINUX)
  target_compile_options(mod_dummy PUBLIC ${C_FLAGS})
endif (BUILD_LINUX)

### GLFW #######################################################################
add_library(mod_glfw SHARED modules/cpr_glfw.c)
set_target_properties(mod_glfw PROPERTIES PREFIX "" OUTPUT_NAME "glfw" SUFFIX "${MODULE_SUFFIX}")
target_compile_definitions(mod_glfw PRIVATE ${CPR_COMPILE_DEF})
if (BUILD_LINUX)
  target_compile_options(mod_glfw PUBLIC ${C_FLAGS})
elseif (BUILD_WIN)
  # Must change output name or import library (.lib) will conflict with the glfw static library (.lib too)
  set_target_properties(mod_glfw PROPERTIES IMPORT_PREFIX "mod_" EXPORT_PREFIX "mod_")
endif (BUILD_LINUX)
# If linking against a static GLFW libary GLFW_LIBRARIES must be used to add
# dependencies (http://www.glfw.org/docs/latest/build.html).
target_link_libraries(mod_glfw glfw ${GLFW_LIBRARIES})

### GL3W #######################################################################
add_library(mod_gl3w SHARED modules/cpr_gl3w.c modules/cpr_gl.c)
set_target_properties(mod_gl3w PROPERTIES PREFIX "" OUTPUT_NAME "gl3w" SUFFIX "${MODULE_SUFFIX}")
target_compile_definitions(mod_gl3w PRIVATE ${CPR_COMPILE_DEF})
if (BUILD_LINUX)
  target_compile_options(mod_gl3w PUBLIC ${C_FLAGS})
elseif (BUILD_WIN)
  set_target_properties(mod_gl3w PROPERTIES IMPORT_PREFIX "mod_" EXPORT_PREFIX "mod_")
endif (BUILD_LINUX)
target_link_libraries(mod_gl3w gl3w)

### IMGUI ######################################################################
add_library(mod_imgui SHARED modules/cpr_imgui.cxx)
set_target_properties(mod_imgui PROPERTIES PREFIX "" OUTPUT_NAME "imgui" SUFFIX "${MODULE_SUFFIX}")
target_compile_definitions(mod_imgui PRIVATE ${CPR_COMPILE_DEF})
if (CPR_BUILD_DEBUG_INTERNAL)
  target_compile_definitions(mod_imgui PRIVATE CPR_DEBUG_INTERNAL=1)
endif(CPR_BUILD_DEBUG_INTERNAL)
if (BUILD_WIN)
  set_target_properties(mod_imgui PROPERTIES IMPORT_PREFIX "mod_" EXPORT_PREFIX "mod_")
endif(BUILD_WIN)
target_link_libraries(mod_imgui imgui)

################################################################################
# INSTALL
################################################################################

install(TARGETS rt cepora mod_dummy mod_glfw mod_gl3w mod_imgui
    BUNDLE  DESTINATION "." COMPONENT Runtime
    RUNTIME DESTINATION ${BUNDLE_RUNTIME_DESTINATION} COMPONENT Runtime
    LIBRARY DESTINATION ${BUNDLE_RUNTIME_DESTINATION} COMPONENT Runtime
    # Don't install static libraries and import lib (windows)
    # ARCHIVE DESTINATION ${BUNDLE_RUNTIME_DESTINATION} COMPONENT Runtime
    )

# On Linux and MacOS we use fixup_bundle. On Windows use
# installrequiredsystemlibraries and manually copy plugins
# directories to search for prerequisites. Libraries are copied automatically
set(DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Plugins are loadable bundles that extend the basic features of your application
# On MacOSX it can be frameworks or shared lib not linked with the executable.
# Plugins must be copied manually before they can be fixed by fixup_bundle.
# get_target_property(LFS_LIB_NAME liblfs OUTPUT_NAME)
# get_target_property(GLFW_LIB_NAME glfw OUTPUT_NAME)
# Freetype CMakeLists.txt does not set any OUTPUT_NAME for the library target
# get_target_property(FREETYPE_LIB_NAME freetype OUTPUT_NAME)

# DLL shared libraries are build in the runtime directory on Windows
if(BUILD_WIN)
   set(LIB_OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
else()
   set(LIB_OUTPUT "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
endif()
# Use shorter variable for clarity
set(LIB_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
set(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})

# Note: no need to add GLFW shared library to the plugin since it's a dependency
# of the glfw module and will be copied by the bundle script.
set(PLUGINS)
list(APPEND PLUGINS "${LIB_OUTPUT}/glfw${MODULE_SUFFIX}")
list(APPEND PLUGINS "${LIB_OUTPUT}/gl3w${MODULE_SUFFIX}")
list(APPEND PLUGINS "${LIB_OUTPUT}/imgui${MODULE_SUFFIX}")

foreach(plugin IN LISTS PLUGINS)
  message(STATUS "Install plugin: " ${PLUGINS})
endforeach()

if(BUILD_WIN)
   # Check the architecture
   if(CMAKE_CL_64)
      if(MSVC_VERSION GREATER 1599)
         # VS 10 and later:
         set(CMAKE_MSVC_ARCH x64)
      else()
         # VS 9 and earlier:
         set(CMAKE_MSVC_ARCH amd64)
      endif()
   else()
      set(CMAKE_MSVC_ARCH x86)
   endif()

   set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNING FALSE)
   set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY FALSE)
   set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP FALSE)
   set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP_NO_WARNINGS OFF)
   set(CMAKE_INSTALL_MFC_LIBRARIES FALSE)
   set(PROG_FILES_DIR "$ENV{ProgramFiles}")
   string(REGEX REPLACE "\\\\" "/" PROG_FILES_DIR ${PROG_FILES_DIR})
   # Copy all the redistribuable C Runtime Libraries (CRT) so we can support pre-Windows10 systems
   # without requestng the user to manually install VCRedist
   # VCRedist may be installed automatically if an installer is used to install the application (see CPack)
   # http://blogs.msdn.com/b/vcblog/archive/2015/03/03/introducing-the-universal-crt.aspx
   file(GLOB CRT_REDIST_DLL "${PROG_FILES_DIR}/Windows Kits/10/Redist/ucrt/DLLs/${CMAKE_MSVC_ARCH}/*.dll")
   # message(STATUS "DEBUG ${CRT_REDIST_DLL}")
   set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS "${CRT_REDIST_DLL}")
   set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${BUNDLE_RUNTIME_DESTINATION}")
   # Using install(CODE) does not work because the cmake environment is not exported
   # install(CODE "include(InstallRequiredSystemLibraries)" COMPONENT Runtime)
   include(InstallRequiredSystemLibraries)
endif(BUILD_WIN)

# Rebase the bundle using configurable cmake script (fixup_bundle.cmake.in)
# Could be done using "install CODE" but it's more flexible using script. Plus get_filename_component
# doesn't seem to work in embedded script ("install CODE").
set(fixup_bundle ${PROJECT_BINARY_DIR}/fixup_bundle.cmake)
message(STATUS fixup_bundle)
configure_file(../cmake/fixup_bundle.cmake.in ${fixup_bundle} @ONLY)
install(SCRIPT ${fixup_bundle} COMPONENT Runtime)
