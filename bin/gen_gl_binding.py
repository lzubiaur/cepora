#!/usr/bin/env python
# coding=utf-8

# Generate OpenGL skeleton binding for duktape

# This file is based on gl3w_eng.py from the gl3w library
# hosted at https://github.com/skaslev/gl3w

# Allow Python 2.6+ to use the print() function
from __future__ import print_function

import re

ignored_functions = []
ignored_defines = []

# Set the first character of the string to lowercase
downcase = lambda s: s[:1].lower() + s[1:] if s else ''

procs = []      # List of functions
enums = []      # List of defines (will be converted into enum in Lua FFI)

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
          'name': 'cpr_'+snake_case(m.group(2)),    # Function name converted to snake case
          'cname': m.group(2),               # Canonical function name (as in the C API)
          'jsname': downcase(m.group(2)[2:]),# Striped function name used as the javascript function name
          'param': m.group(3) }              # Function parameter list (including the brackets)
    if any(f == t['cname'] for f in ignored_functions):
        return
    procs.append(t)

def def_t(m):
    t = { 'name': m.group(1)[3:],            # The `GL_` prefix is removed
          'cname': m.group(1),               # Canonical define name
          'value': m.group(2) }
    if any(f == t['cname'] for f in ignored_defines):
        return
    enums.append(t)

# Regex for functions, defines and typdefs
p = [ [ re.compile(r'GLAPI\s(.*)\sAPIENTRY\s(\w+)\s\((.*)\);'), func_t ],
      [ re.compile(r'#define\s*(GL_\w+)\s*(-?\w+)'), def_t ] ]

# Parse function names, defines and typedefs from glcorearb.h
print('Parsing OpenGL header...')
with open('lib/gl3w/include/GL/glcorearb.h', 'r') as f:
    for line in f:
        for _p in p:
            m = _p[0].match(line)
            if m:
                _p[1](m)
                continue

procs.sort()
enums.sort()

with open('build/gl_sekeleton.h', 'wb') as f:
    f.write('''/*
 * cpr_gl.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

/* OpenGL binding sekeleton automatically generated using {0} */

#include "cpr_gl.h"
#include "GL/gl3w.h"

''')
    for p in procs:
        return_count = 1 if p['rt'] != 'void' else 0
        f.write('duk_ret_t {0[name]}(duk_context *ctx) {{\n'.format(p).encode('utf-8'))
        f.write('/* {0[rt]} {0[cname]}({0[param]}); */\n'.format(p).encode('utf-8'))
        f.write('  return {0};\n}}\n\n'.format(return_count).encode('utf-8'))

    f.write('''
duk_ret_t dukopen_gl(duk_context *ctx) {
  const duk_number_list_entry module_consts[] = {
''')
    for e in enums:
        f.write('  /* {{ {0:<30} {1:<5} }}, */\n'.format( '"'+e['name']+'",', '(double) '+e['cname']).encode('utf-8'))
    f.write('''  { NULL, 0.0 }
  };

  const duk_function_list_entry module_funcs[] = {
''')
    # Functions list
    for p in procs:
        param_count = p['param'].count(',') + 1 if p['param'] != 'void' else 0
        f.write('  /* {{ {0:<30} {1:<35} {2:<3} }}, */\n'.format('"'+p['jsname']+'",', p['name']+',', param_count).encode('utf-8'))
    f.write('''  { NULL, NULL, 0 }
};

  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);
  return 1;
}

''')

print('Parsed {0} defines.'.format(len(enums)))
print('Parsed {0} functions.'.format(len(procs)))
print('Output written in build/gl_skeleton.h.')
