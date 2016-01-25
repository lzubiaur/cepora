/*
 * cpr_loadlib.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

/* Module docs:
 * https://github.com/svaarala/duktape/blob/master/doc/c-module-convention.rst
 * https://github.com/svaarala/duktape/blob/master/doc/modules.rst
 * http://wiki.duktape.org/HowtoModules.html
 */

#include "cpr_config.h"

#include "duktape.h"
#include "cpr_loadlib.h"
#include "cpr_macros.h"

#define CPR_OPEN_PREFIX "dukopen_"
#define CPR_OPEN_PREFIX_LEN (sizeof(INIT_PREFIX)-1)

static void cpr_close_lib(void *handle);
static void *cpr_open_lib(duk_context *ctx, const char *filename);
static duk_c_function cpr_load_sym(duk_context *ctx, void *handle, const char *sym);

#if defined(CPR_USE_DLOPEN)

#include <dlfcn.h>

void cpr_close_lib(void *handle) {
  if (handle) {
    dlclose(handle);
  }
}

void *cpr_open_lib(duk_context *ctx, const char *filename) {
  void *handle = NULL;
  /* From Linux man page:
  * RTLD_NOW: all undefined symbols in the library are resolved before dlopen
  * RTLD_LOCAL:  Symbols defined in this library are not made available to
  * resolve references in subsequently loaded libraries */
  handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
  if (!handle) {
    /* Low level error is logged and a custom error message is returned so we
     * can look for the same message on every platform (mainly for testing purpose). */
    ERR(ctx, "%s", dlerror());
    duk_push_sprintf(ctx, "Cannot open shared library '%s'", filename);
  }

  return handle;
}

duk_c_function cpr_load_sym(duk_context *ctx, void *handle, const char *sym) {
  duk_c_function func = NULL;
  char *errmsg = NULL;

  if (handle == NULL) {
    duk_push_string(ctx, "Invalid library handle (NULL)");
    return NULL;
  }

  /* Clear any existing previous error */
  dlerror();
  func = (duk_c_function) dlsym(handle, sym);
  if ((errmsg = dlerror()) != NULL)  {
    ERR(ctx, "%s", errmsg);
    duk_push_sprintf(ctx, "Cannot find symbol '%s'", sym);
    return NULL;
  }
  return func;
}

#else
#error Dynamic library loading not supported on this platform
#endif

/* Check if the module `mod_id` is already loaded. */
static duk_ret_t cpr_is_mod_loaded(duk_context *ctx, const char *mod_id) {
  duk_ret_t rc = 0;
  duk_get_global_string(ctx, "Duktape");
  duk_get_prop_string(ctx, -1, "modLoaded");
  duk_push_string(ctx, mod_id);
  rc = duk_has_prop(ctx, -2);
  duk_pop_2(ctx);

  return rc;
}

/* Low level library loading */
duk_ret_t cpr_loadlib(duk_context *ctx) {
  void *lib = NULL;
  const char *dot = NULL;
  const char *filename = NULL;
  const char *id = NULL;
  // const char *modid = NULL;
  duk_size_t len = 0;
  duk_c_function init_func = NULL;

  filename = duk_require_string(ctx, 0);
  id = duk_require_string(ctx, 1);

  DBG(ctx, "cpr_loadlib: id: '%s' filename '%s'", id, filename);

  if (cpr_is_mod_loaded(ctx, id) == 1) {
    WRN(ctx, "FIXME - Module already loaded: '%s'", id);
    /* FIXME Checking it modLoaded has the property `filename` is not
     * enough because:
     * 1: the module id must be extract from the path
     * 2: the property already exists when the require handler is called
     */
  }

  if ((lib = cpr_open_lib(ctx, filename)) == NULL) {
    goto error;
  }

  if ((dot = strrchr(id, '.')) == NULL) {
    len = strlen(id);
  } else {
    len = dot - id;
  }
  duk_push_string(ctx, CPR_OPEN_PREFIX);
  duk_push_lstring(ctx, id, len);
  duk_concat(ctx, 2);
  if ((init_func = cpr_load_sym(ctx, lib, duk_get_string(ctx, -1))) == NULL) {
    /* TODO test error */
    goto error;
  }
  duk_pop(ctx);
  /* Call the module's init function */
  duk_push_c_function(ctx, init_func, 0);
  if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS ) {
    /* TODO test error */
    goto err_rethrow;
  }
  return 1; /* Return the module init function result */

error:
  cpr_close_lib(lib);
  duk_error(ctx, 1, duk_get_string(ctx, -1));
  return 0; /* Not reachable */

err_rethrow:
  cpr_close_lib(lib);
  duk_throw(ctx);
  return 0; /* Not reachable */
}

static const duk_function_list_entry module_funcs[] = {
    { "loadlib", cpr_loadlib, 2 },
    { NULL, NULL, 0 }
};

duk_ret_t dukopen_loadlib(duk_context *ctx) {
  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);

  return 1;  /* return module value */
}
