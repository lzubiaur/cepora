### @test
###

try
  glfw  = require 'glfw.so'
  gl3w  = require 'gl3w.so'
  imgui = require 'imgui.so'

  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  glfw.windowHint glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE
  glfw.windowHint glfw.CONTEXT_VERSION_MAJOR, 3
  glfw.windowHint glfw.CONTEXT_VERSION_MINOR, 2
  glfw.windowHint glfw.OPENGL_FORWARD_COMPAT, 1 if Duktape.os == 'osx'

  window = glfw.createWindow 480, 320, 'my window'
  throw new Error 'Cannot create OpenGL window' if not window
  glfw.makeContextCurrent window

  throw new Error 'Cannot init gl3w' if gl3w.init()

  imgui.getIO()

catch e
  print e.message
