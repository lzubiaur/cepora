log = new Duktape.Logger('module.coffee')
inf = () -> log.info.apply log, arguments
err = () -> log.error.apply log, arguments

glfw = require 'glfw.so'

errorHandler = (code, message) -> err code, message

keyHandler = (window, key, scancode, action, mods) ->
  # if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  glfw.setWindowShouldClose window, true if key == glfw.KEY_ESCAPE
  inf key, scancode, action, mods
  # test remove key callback
  glfw.setKeyCallback window, null

windowPosHandler = (window, x, y) -> inf 'New window position:', x, y
windowSizeHandler = (window, w, h) -> inf 'New window size:', w, h
windowCloseHandler = (window) -> inf 'Window will close'
windowRefreshHandler = (window) ->
  inf 'Window refresh'
  glfw.swapBuffers window
windowFocusHandler = (window, focused) -> inf 'Window focus:', focused
windowIconifyHandler = (window, iconified) -> inf 'Window iconify:', iconified
frameBufferSizeHandler = (window, w, h) -> inf 'New framebuffer size:', w, h

mainLoop = (window) ->
  glfw.pollEvents()
  glfw.swapBuffers window

try
  inf 'GLFW version ', glfw.VERSION_MAJOR + '.' + glfw.VERSION_MINOR + '.' + glfw.VERSION_REVISION

  rc = glfw.init()
  throw new Error 'Cannot initialize GLFW library' if not rc

  glfw.setErrorCallBack errorHandler

  #### Window handling ####
  # glfw.windowHint glfw.RESIZABLE, 0
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

  # glfwSetWindowPosCallback
  glfw.setWindowPosCallback window, windowPosHandler
  glfw.setWindowSizeCallback window, windowSizeHandler
  glfw.setWindowCloseCallback window, windowCloseHandler
  glfw.setWindowRefreshCallback window, windowRefreshHandler
  glfw.setWindowFocusCallback window, windowFocusHandler
  glfw.setWindowIconifyCallback window, windowIconifyHandler
  glfw.setFramebufferSizeCallback window, frameBufferSizeHandler

  #### Monitor handling ####
  monitor = glfw.getPrimaryMonitor()
  inf 'Monitors:', glfw.getMonitors()
  inf 'Monitor position:', glfw.getMonitorPos monitor
  inf 'Monitor physical size:', glfw.getMonitorPhysicalSize monitor
  inf 'Monitor name:', glfw.getMonitorName monitor
  # Not implemented
  # glfw.setMonitorCallback()
  inf 'Monitor video mode:', o for o in glfw.getVideoModes monitor
  inf 'Monitor current mode:', glfw.getVideoMode monitor
  inf 'Monitor gamma ramp:', glfw.getGammaRamp monitor
  # TODO test with actual data
  red = (num for num in [1..256])
  green = (num for num in [1..256])
  blue = (num for num in [1..256])
  glfw.setGammaRamp monitor, [red, green, blue]

  #### Input handling ####
  glfw.setInputMode window, glfw.CURSOR, glfw.CURSOR_NORMAL
  glfw.setInputMode window, glfw.STICKY_KEYS, 1
  glfw.setInputMode window, glfw.STICKY_MOUSE_BUTTONS, 1
  inf '[Input mode] Cursor:', glfw.getInputMode window, glfw.CURSOR
  inf '[Input mode] Sticky keys:', glfw.getInputMode window, glfw.STICKY_KEYS
  inf '[Input mode] Sticky mouse buttons:', glfw.getInputMode window, glfw.STICKY_MOUSE_BUTTONS
  inf '[getKey] KEY_K:', glfw.getKey window, glfw.KEY_K
  inf '[getMouseButton] MOUSE_BUTTON_1:', glfw.getMouseButton window, glfw.MOUSE_BUTTON_1
  # TODO setCursorPos doesn't seem to work
  glfw.setCursorPos window, 100, 100
  inf '[getCursorPos]:', glfw.getCursorPos window
  cursor = glfw.createStandardCursor glfw.HAND_CURSOR
  glfw.setCursor window, cursor

  # TODO test createCursor with actual 32bits pixel data
  # Create a plain buffer of 8 bytes
  buffer = Duktape.Buffer 64
  # Fill it using index properties
  buffer[i] = 0x0f +i for i in [i..buffer.length]
  cursor = glfw.createCursor buffer, 32, 32, 0, 0
  glfw.setCursor window, cursor

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

  glfw.setKeyCallback window, keyHandler

  mainLoop window while not glfw.windowShouldClose window

catch error
  err error.stack
finally
  inf 'clean up...'
  glfw.terminate()
