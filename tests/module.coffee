print o for o in Duktape.arguments

# Load Javascript module
require 'js/tests/test.coffee'

# Load C module
io = require('libio.dylib')

print io.DELAY
io.read()

# Print currently loaded modules
print o for o in Duktape.modLoaded
