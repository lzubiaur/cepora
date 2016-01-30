log = new Duktape.Logger('glfw.coffee')
inf = () -> log.info.apply log, arguments
err = () -> log.error.apply log, arguments

glfw = require 'glfw.so'

try
  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  #### Window handling ####
  # glfw.windowHint glfw.RESIZABLE, 0
  # glfwCreateWindow
  window = glfw.createWindow 480, 320, 'my window'
  # Full screen
  throw new Error 'error window' if not window

  # the main loop
  while not glfw.windowShouldClose window
    glfw.pollEvents()
    glfw.swapBuffers window

catch error
  err error.stack
finally
  inf 'clean up...'
  glfw.terminate()
