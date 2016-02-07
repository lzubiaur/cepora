### @test
480 320
60
100.3
50.9
###

glfw  = require 'glfw.so'
gl  = require 'gl3w.so'
imgui = require 'imgui.so'
io = imgui.getIO()

mainLoop = (window) ->
  glfw.pollEvents()
  gl.clearColor 0.3, 0.28, 0.35, 1
  gl.clear gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT
  imgui.newFrame()
  imgui.showUserGuide()
  imgui.showTestWindow()
  imgui.begin 'my window'
  imgui.text 'hello', ' world'
  imgui.textColored 100, 100, 0, 255, 'colored', ' text'
  imgui.textDisabled 'disabled text'
  imgui.textWrapped 'wrapped text'
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

  throw new Error 'Cannot init gl3w' if gl.init()

  throw new Error 'Cannot init imgui' if !imgui.init window, false

  io.displaySize.x = 480
  io.displaySize.y = 320
  print io.displaySize.x, io.displaySize.y

  mainLoop window while not glfw.windowShouldClose window

catch e
  print e.message
