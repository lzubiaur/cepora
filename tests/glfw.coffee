log = new Duktape.Logger('module.coffee')
inf = () -> log.info.apply log, arguments
err = () -> log.error.apply log, arguments

error_handler = (err, message) -> print err, message

glfw = require 'glfw.so'

try
  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  glfw.setErrorCallBack error_handler

  window = glfw.createWindow(640, 480, "test")
  throw new Error 'error window' if not window
  glfw.makeContextCurrent(window)

  print '' while not glfw.windowShouldClose window

catch error
  err error.stack
finally
  glfw.terminate()
  print 'clean up...'
