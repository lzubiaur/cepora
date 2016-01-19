log = new Duktape.Logger('module.coffee')
info = () -> log.info.apply log, arguments

# Load Javascript module
# require 'js/tests/test.coffee'
info 'Loaded modules:'
info o for o of Duktape.modLoaded

# Search for a dummy file
try
  require 'dummy_file.txt'
catch e
  info e.stack

try
  require 'js/tests/broken.coffee'
catch e
  info e.stack

# Load C module
try
  io = require 'io.so'
  info io.DELAY
  io.read()
  # io = require s
catch e
  info e.stack

# Print currently loaded modules
info 'Loaded modules:'
info o for o of Duktape.modLoaded
