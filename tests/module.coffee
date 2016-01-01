print o for o in Duktape.arguments

require 'js/tests/test.coffee'

io = require('libio.dylib')

print io.DELAY
print typeof io.read
io.read()


# Print currently loaded modules
print o for o in Duktape.modLoaded

print o for o of this
