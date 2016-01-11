log = new Duktape.Logger('module.coffee')
inf = () -> log.info.apply log, arguments
err = () -> log.error.apply log, arguments

glfw = require 'glfw.so'

error_handler = (code, message) -> err code, message

key_handler = (window, key, scancode, action, mods) ->
  # if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  glfw.setWindowShouldClose window, true if key == glfw.KEY_ESCAPE
  inf key, scancode, action, mods

main_loop = (window) ->
  glfw.pollEvents()
  glfw.swapBuffers window

try
  inf 'GLFW version ', glfw.VERSION_MAJOR + '.' + glfw.VERSION_MINOR + '.' + glfw.VERSION_REVISION

  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  glfw.setErrorCallBack error_handler

  #### Window handling ####
  glfw.windowHint glfw.RESIZABLE, 0
  # glfwCreateWindow
  window = glfw.createWindow 480, 320, 'my window'
  # Full screen
  # window = glfw.createWindow 640, 480, 'my window', glfw.getPrimaryMonitor()
  throw new Error 'error window' if not window
  # glfwSetWindowTitle
  glfw.setWindowTitle window, 'another title'
  # glfwSetWindowSize
  glfw.setWindowSize window, 640, 480
  # glfwSetWindowPos
  glfw.setWindowPos window, 100, 200
  # glfwGetWindowPos
  inf 'Window position:', glfw.getWindowPos window
  # glfwGetWindowSize
  inf 'Window size:', glfw.getWindowSize window
  # glfwGetWindowSize
  inf 'Framebuffer size:', glfw.getFramebufferSize window
  # glfwGetWindowFrameSize
  inf 'Window Frame size', glfw.getWindowFrameSize window
  # glfwIconifyWindow
  glfw.iconifyWindow window
  # glfwRestoreWindow
  glfw.restoreWindow window
  # glfwHideWindow
  glfw.hideWindow window
  # glfwShowWindow
  glfw.showWindow window
  # glfwGetWindowMonitor
  inf 'Window monitor (null if not fullscreen): ', glfw.getWindowMonitor window
  # glfwGetWindowAttrib
  inf 'Window is resizable: ', glfw.getWindowAttrib window, glfw.RESIZABLE
  # glfwSetWindowUserPointer/glfwGetWindowUserPointer
  str = 'my user data'
  glfw.setWindowUserPointer window, str
  inf glfw.getWindowUserPointer window

  # TODO test glfwGetWindowUserPointer/glfwSetWindowUserPointer

  #### Context handling ####
  # glfwMakeContextCurrent
  glfw.makeContextCurrent window
  # glfwGetCurrentContext
  inf 'getCurrentContext:', glfw.getCurrentContext(), window

  # glfwSwapInterval
  glfw.swapInterval 1
  # Test manual extension loading if enabled
  if glfw.extensionSupported?
    inf 'swap control tear is supported' if glfw.extensionSupported 'WGL_EXT_swap_control_tear'
    # glfwGetProcAddress
    inf glfw.getProcAddress 'glGetDebugMessageLogARB'

  # Test version binding if enabled
  if glfw.getVersion?
    inf glfw.getVersionString()
    inf glfw.getVersion()

  glfw.setKeyCallback window, key_handler

  main_loop window while not glfw.windowShouldClose window

catch error
  err error.stack
finally
  inf 'clean up...'
  glfw.terminate()
