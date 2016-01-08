log = new Duktape.Logger('module.coffee')
inf = () -> log.info.apply log, arguments
err = () -> log.error.apply log, arguments

glfw = require 'glfw.so'

error_handler = (err, message) -> print err, message

key_handler = (window, key, scancode, action, mods) ->
  # if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  glfw.setWindowShouldClose window, true if key == glfw.GLFW_KEY_ESCAPE
  print key, scancode, action, mods

main_loop = (window) ->
  glfw.poolEvents()
  glfw.swapBuffers window

try
  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  glfw.setErrorCallBack error_handler

  window = glfw.createWindow(640, 480, "test")
  throw new Error 'error window' if not window

  glfw.makeContextCurrent window

  glfw.setKeyCallback window, key_handler

  main_loop window while not glfw.windowShouldClose window

catch error
  err error.stack
finally
  print 'clean up...'
  glfw.terminate()
