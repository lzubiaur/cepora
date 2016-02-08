log = new Duktape.Logger('glfw.coffee')
inf = () -> log.info.apply log, arguments
err = () -> log.error.apply log, arguments

glfw = require 'glfw.so'
gl   = require 'gl3w.so'

refreshWindow = (window) ->
  gl.clearColor 0, 0, 0, 1
  gl.clear gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT
  glfw.swapBuffers window

resizeWindow = (window, w, h) ->
  gl.viewport 0, 0, w, h
  gl.clearDepth 1.0
  gl.clearColor 0, 0, 0, 0
  gl.enable gl.DEPTH_TEST

try
  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  # at least OpenGL 3 is required by gl3w
  glfw.windowHint glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE
  glfw.windowHint glfw.CONTEXT_VERSION_MAJOR, 3
  glfw.windowHint glfw.CONTEXT_VERSION_MINOR, 2
  glfw.windowHint glfw.OPENGL_FORWARD_COMPAT, 1 if Duktape.os == 'osx'
  window = glfw.createWindow 480, 320, 'cepora', null, null
  throw new Error 'Cannot create window' if not window

  glfw.makeContextCurrent window

  rc = gl.init()
  throw new Error 'Cannot init GL3W' if not rc

  glfw.setWindowRefreshCallback window, refreshWindow
  glfw.setFramebufferSizeCallback window, resizeWindow

  # the main loop
  while not glfw.windowShouldClose window
    glfw.pollEvents()
    refreshWindow window

catch error
  err error.stack
finally
  glfw.terminate()
