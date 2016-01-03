/*
 * cpr_loadlib.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "duktape.h"
#include "cpr_config.h"
#include "cpr_loadlib.h"

static void cpr_unload_lib(void *handle);
static void *cpr_load_lib(duk_context *ctx, const char *filename);
static duk_c_function cpr_load_sym(duk_context *ctx, void *handle, const char *sym);

#if defined(CPR_USE_DLOPEN)

#include <dlfcn.h>

void cpr_unload_lib(void *handle)
{
  if (handle) {
    dlclose(handle);
  }
}

void *cpr_load_lib(duk_context *ctx, const char *filename)
{
  void *handle = NULL;
  /* From Linux man page:
  * RTLD_NOW: all undefined symbols in the library are resolved before dlopen
  * RTLD_LOCAL:  Symbols defined in this library are not made available to
  * resolve references in subsequently loaded libraries */
  handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
  if (!handle) {
    duk_push_sprintf(ctx, "Cannot open module '%s': %s", filename, dlerror());
  }

  return handle;
}

#else
#error Dynamic library loading not supported on this platform
#endif
