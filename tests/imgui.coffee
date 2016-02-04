### @test
480 320
60
100.3
50.9
###

glfw  = require 'glfw.so'
gl3w  = require 'gl3w.so'
imgui = require 'imgui.so'
io = imgui.getIO()

mainLoop = (window) ->
  glfw.pollEvents()
  imgui.newFrame()
  imgui.begin 'my window'
  imgui.text 'hello world'
  imgui.end()
  imgui.render()
  glfw.swapBuffers window

errorHandler = (code, message) -> print code, message

try
  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  glfw.setErrorCallBack errorHandler

  glfw.windowHint glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE
  glfw.windowHint glfw.CONTEXT_VERSION_MAJOR, 3
  glfw.windowHint glfw.CONTEXT_VERSION_MINOR, 2
  glfw.windowHint glfw.OPENGL_FORWARD_COMPAT, 1 if Duktape.os == 'osx'

  window = glfw.createWindow 480, 320, 'my window'
  throw new Error 'Cannot create OpenGL window' if not window
  glfw.makeContextCurrent window

  throw new Error 'Cannot init gl3w' if gl3w.init()

  throw new Error 'Cannot init imgui' if !imgui.init window, true

  io.displaySize.x = 480
  io.displaySize.y = 320
  print io.displaySize.x, io.displaySize.y

  mainLoop window while not glfw.windowShouldClose window

catch e
  print e.message
