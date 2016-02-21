### @test
Cannot open shared library '/tmp/dummy.so'
Cannot find symbol 'dukopen_foo'
-1
hello world
###

try
  lib.loadlib '/tmp/dummy.so', 'dummy'
catch e
  print e.message

try
  path = module.searchPath 'dummy.so'
  lib.loadlib path, 'foo'
catch e
  print e.message

try
  path = module.searchPath 'dummy.so'
  dummy = lib.loadlib path, 'dummy'
  print dummy.BAR
  dummy.foo 'hello world\n'
catch e
  print e.message
