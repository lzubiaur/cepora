# Assignment:
number   = 42
opposite = true

# Conditions:
number = -42 if opposite

alert "I knew it!", number

square = (x) -> x * x
cube   = (x) -> square(x) * x

alert cube 2

if true
  alert 'true'
else
  alert 'unless'

eat = (x) -> alert 'miam', x

eat food for food in ['toast', 'cheese', 'wine']

menu = (a...) -> alert a

courses = ['greens', 'caviar', 'truffles', 'roast', 'cake']
menu i + 1, dish for dish, i in courses

# Iterate over the key and values in a object
object = foo: 10, bar: 9

p = for key, value of object
  "#{key} is #{value}"
alert p

# Nursery Rhyme
num = 6
lyrics = while num -= 1
  "#{num} little monkeys, jumping on the bed.
    One fell out and bumped his head."

alert lyrics

version = Duktape.version
major = Math.floor version / 10000
minor = Math.floor (version - major * 10000) / 100
patch = version - major * 10000 - minor * 100

print 'Duktape '.concat major, '.', minor, '.', patch

for _,b of Duktape.argv
  print b
