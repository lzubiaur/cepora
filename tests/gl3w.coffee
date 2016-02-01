### @test
0
true
###

try
  glfw = require 'glfw.so'
  gl3w = require 'gl3w.so'

  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  glfw.windowHint glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE
  glfw.windowHint glfw.CONTEXT_VERSION_MAJOR, 3
  glfw.windowHint glfw.CONTEXT_VERSION_MINOR, 2
  glfw.windowHint glfw.OPENGL_FORWARD_COMPAT, 1 if Duktape.os == 'osx'

  window = glfw.createWindow 480, 320, 'my window'
  throw new Error 'Cannot create OpenGL window' if not window
  glfw.makeContextCurrent window

  # GL context must be initialized (windowHint), created (createWindow) and
  # make current (makeContextCurrent) before gl3w.init can be called
  print gl3w.init()
  print gl3w.isSupported 3, 2

catch e
  print e.message
