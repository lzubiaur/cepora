#!/usr/bin/env python
# coding=utf-8

# This file is based on gl3w_eng.py from the gl3w library
# hosted at https://github.com/skaslev/gl3w

# Allow Python 2.6+ to use the print() function
from __future__ import print_function

import re
import sys,os

ignored_functions = []
ignored_defines = []
ignored_typdefs = []

# Set the first character of the string to lowercase
downcase = lambda s: s[:1].lower() + s[1:] if s else ''

procs = []      # List of functions
enums = []      # List of defines (will be converted into enum in Lua FFI)
typedefs = []   # List ot typedef

# Convert string from CamelCase to snake_case
# http://stackoverflow.com/questions/1175208/elegant-python-function-to-convert-camelcase-to-camel-case
first_cap_re = re.compile('(.)([A-Z][a-z]+)')
all_cap_re = re.compile('([a-z0-9])([A-Z])')
def snake_case(name):
    s1 = first_cap_re.sub(r'\1_\2', name)
    return all_cap_re.sub(r'\1_\2', s1).lower()

# Split a regular expression into
def func_t(m):
    t = { 'rt': m.group(1),                  # Return type
          'name': snake_case(m.group(2)),    # function name converted to snake case
          'cname': m.group(2),               # Canonical function name (as in the C API)
          'jsname': downcase(m.group(2)[4:]),# striped funciton name used as the javascript function name
          'param': m.group(3) }              # Function parameter list (including the brackets)
    if any(f == t['cname'] for f in ignored_functions):
        return
    procs.append(t)

def def_t(m):
    t = { 'name': m.group(1)[5:],            # The `GLFW_` prefix is removed
          'cname': m.group(1),               # Canonical define name
          'value': m.group(2) }
    if any(f == t['cname'] for f in ignored_defines):
        return
    enums.append(t)

def typedef_t(m):
    t = { 'name': m.group(2),
          'type': m.group(1) }
    if any(f in m.group(0) for f in ignored_typdefs):
        return
    typedefs.append(t)

# Regex for functions, defines and typdefs
p = [ [ re.compile(r'GLFWAPI\s(.*)\s(\w+)\((.*)\);'), func_t ],
      [ re.compile(r'#define\s*(GLFW_\w+)\s*(-?\w+)'), def_t ],
      [ re.compile(r'typedef(.*)(GL.*);'), typedef_t] ]

# Parse function names, defines and typedefs from glcorearb.h
print('Parsing glfw3.h header...')
with open('lib/glfw/include/GLFW/glfw3.h', 'r') as f:
    for line in f:
        for _p in p:
            m = _p[0].match(line)
            if m:
                _p[1](m)
                continue

# procs.sort()
# enums.sort()
# Remove duplicates
# typedefs = list(set(typedefs))
# typedefs.sort()

with open('build/glfw_sekeleton.h', 'wb') as f:
    f.write('''
/* Automatically generated using {0} */

/* GLFW typedef */
'''.format(__file__))
    for t in typedefs:
        f.write('typedef {0[type]} {0[name]};\n'.format(t).encode('utf-8'))
    f.write('''
/* GLFW defines */
const duk_number_list_entry module_consts[] = {
''')
    for e in enums:
        f.write('  {{ {0:<30} {1:<5} }},\n'.format( '"'+e['name']+'",', '(double) '+e['cname']).encode('utf-8'))
    f.write('''  { NULL, NULL, 0 }
};

/* GLFW functions */
''')
    # Function binding skeleton
    for p in procs:
        return_count = 1 if p['rt'] != 'void' else 0
        f.write('\nduk_ret_t {0[name]}(duk_context *ctx) {{'.format(p).encode('utf-8'))
        f.write('\n  /* {0[rt]} {0[cname]}({0[param]}); */'.format(p).encode('utf-8'))
        f.write('\n  return {0};\n}}\n'.format(return_count).encode('utf-8'))

    # Functions list
    for p in procs:
        param_count = p['param'].count(',') + 1 if p['param'] != 'void' else 0
        f.write('  {{ {0:<30} {1:<35} {2:<3} }},\n'.format('"'+p['jsname']+'",', p['name']+',', param_count).encode('utf-8'))

print('Parsed {0} typedefs.'.format(len(typedefs)))
print('Parsed {0} defines.'.format(len(enums)))
print('Parsed {0} functions.'.format(len(procs)))
print('Output written in build/glfw_skeleton.h.')
