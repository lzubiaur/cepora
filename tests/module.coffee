### @test
module '_NOT_EXISTS.so' not found
Can't compile script broken.coffee
-1
Hello world
dummy.so
###

# Load Javascript module
# require 'js/tests/hello.js'

try
  require '_NOT_EXISTS.so'
catch e
  print e.message

try
  require 'js/tests/broken.coffee'
catch e
  print "Can't compile script broken.coffee"

try
  lib.loadlib 'dldl.so', 'ddd'
catch e
  print e.message

try
  path = module.searchPath 'dummy.so'
  lib.loadlib path, 'dummy'
catch e
  print e.message

# Load C module
try
  dummy = require 'dummy.so'
  print dummy.BAR
  dummy.foo 'Hello world\n'
  # io = require s
catch e
  print e.message

# Print currently loaded modules
print o for o of Duktape.modLoaded
