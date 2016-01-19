/*
 * cpr_package.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_config.h"
#include "cpr_package.h"
#include "cpr_debug_internal.h"
#include "cpr_sys_tools.h"
#include "cpr_error.h"
#include "cpr_macros.h"
#include "cpr_loadlib.h"

#include <stdlib.h> /* getenv */

#define CPR__PATH_SEPARATOR ';'

/* Look up for a file using the search paths (package.paths).
 */
static duk_ret_t cpr__search_path(duk_context *ctx) {
  if (duk_is_null_or_undefined(ctx, -1)) {
    CPR__DLOG("parameter `filename` is undefined");
    duk_push_undefined(ctx);
    return 1;
  }
  duk_get_global_string(ctx, "package");
  duk_get_prop_string(ctx, -1, "paths");
  duk_enum(ctx, -1, DUK_ENUM_ARRAY_INDICES_ONLY);
  while (duk_next(ctx, -1, 1)) {
    duk_push_string(ctx, "/");
    duk_dup(ctx, 0);
    duk_concat(ctx, 3);
    if (cpr_file_exists(duk_get_string(ctx, -1))) {
      DBG(ctx, "File found '%s'", duk_get_string(ctx, -1));
      return 1;
    }
    WRN(ctx, "File NOT found '%s'", duk_get_string(ctx, -1));
    duk_pop_2(ctx); /* pop key and value */
  }
  duk_pop(ctx); /* enum */
  duk_pop_3(ctx); /* Duktape package paths */
  ERR(ctx, "File NOT found '%s'", duk_get_string(ctx, 0));
  return 1;
}

/* Custom package loader
 * @params id, require, exports, module
 */
static duk_ret_t cpr__require_handler(duk_context *ctx) {
  const char *filename = NULL;
  /* Search for the file in the search paths */
  duk_get_global_string(ctx, "package");
  duk_get_prop_string(ctx, -1, "searchPath");
  duk_dup(ctx, 0);
  duk_call(ctx, 1);
  filename = duk_get_string(ctx, -1);
  /* TODO Lazy file extension check  */
  char *dot = strrchr(duk_get_string(ctx, -1), '.');
  if (dot && strcmp(dot, ".coffee") == 0) {
    INF(ctx, "Load CoffeeScript module '%s'", filename);
    duk_get_global_string(ctx, "coffee");
    duk_push_string(ctx, "compile_coffee");
    duk_push_string(ctx, filename);
    if (duk_pcall_prop(ctx, -3, 1) != DUK_EXEC_SUCCESS) {
      // duk_error(ctx, DUK_ERR_RANGE_ERROR, "argument out of range: %d", (int) argval);
      ERR(ctx, "Cannot compile CoffeeScript '%s'", filename);
      cpr_dump_stack_trace(ctx, -1);
    }
  } else if (dot && strcmp(dot, ".so") == 0) {
    INF(ctx, "Load C module id: '%s' filename:'%s'", duk_get_string(ctx, 0), filename);
    duk_push_c_function(ctx, cpr_loadlib, 2);
    duk_push_string(ctx, filename);
    duk_dup(ctx, 0);
    duk_call(ctx, 2);
    /* duk_replace(ctx, 2);*/  /* Replacing the "exports" table doesnt work */
    /* The init function should return (push) an object with the exported
    * functions/properties. Those exported functions are then copied to the
    * `exports` table so they are available outside the C module.
    */
    duk_enum(ctx, -1, DUK_ENUM_INCLUDE_NONENUMERABLE | DUK_ENUM_OWN_PROPERTIES_ONLY | DUK_ENUM_INCLUDE_INTERNAL);
    while (duk_next(ctx, -1 /*enum_index*/, 1 /*get_value*/)) {
      duk_put_prop(ctx, 2); /* `exports` table is the third parameters (at idx 2 on the stack) */
    }
    duk_pop(ctx); /* pop enum object */
    duk_pop(ctx); /* pop module result */
    duk_push_undefined(ctx); /* Return undefined because no source code. */
  } else {
    INF(ctx, "Load Javascript module '%s'", filename);
    duk_push_string_file(ctx, filename);
  }

  return 1;
}

/* Look for CPR_PATH environment variable to set the search paths. If CPR_PATH
 * is not defined, try to figure out the search paths from the process
 * executable path.
 */
static duk_ret_t cpr__init_search_path(duk_context *ctx) {
  const char *ptr = NULL, *ptr2 = NULL, *path = NULL;
  int i = 0;
  duk_idx_t obj_idx;

  obj_idx = duk_normalize_index(ctx, -1);

  if ((path = getenv("CPR_PATH")) != NULL) {
    INF(ctx, "Found CPR_PATH : '%s'\n", path);
    ptr = path;
    duk_push_array(ctx);
    while((ptr2 = strchr(ptr, CPR__PATH_SEPARATOR)) != NULL) {
      if ((ptr2 - ptr) > 0 ) {
        duk_push_lstring(ctx, ptr, (duk_size_t)(ptr2 - ptr));
        duk_put_prop_index(ctx, -2, i++);
      }
      ptr = ptr2 + 1;
    }
    if (strlen(ptr) > 0) {
      duk_push_string(ctx, ptr);
      duk_put_prop_index(ctx, -2, i++);
    }
    duk_put_prop_string(ctx, obj_idx, "paths");
  } else if ((path = cpr_get_exec_dir()) != NULL) {
    CPR__DLOG("executable dir: '%s'", path);
    duk_push_array(ctx);
    duk_push_string(ctx, path);
    duk_put_prop_index(ctx, -2, 0);
    /* TODO Add Resources folder for MacOS platform */
    duk_push_string(ctx, path);
    duk_push_string(ctx, "/../Resources");
    duk_concat(ctx, 2);
    duk_put_prop_index(ctx, -2, 1);
    duk_put_prop_string(ctx, obj_idx, "paths");
  } else {
    // duk_error requires using variable parameter so we use the duk_push_string/duk_throw workaround.
    duk_push_string(ctx, "Can't retreive executable path. Please try setting CPR_PATH.");
    duk_throw(ctx);
  }
  return 0;
}

static const duk_function_list_entry module_funcs[] = {
    { "searchPath", cpr__search_path, 1 },
    { NULL, NULL, 0 }
};

const duk_number_list_entry module_consts[] = {
    // { "FLAG_FOO", (double) (1 << 0) },
    { NULL, 0.0 }
};

duk_ret_t dukopen_package(duk_context *ctx) {
  CPR__DLOG("Opening module");
  duk_push_object(ctx);  /* module object, must be created before calling cpr__init_search_path */
  cpr__init_search_path(ctx);
  /* Set the module search function */
  duk_get_global_string(ctx, "Duktape");
  duk_push_c_function(ctx, cpr__require_handler, 4 /* parameters */);
  duk_put_prop_string(ctx, -2, "modSearch");
  duk_pop(ctx); /* pop Duktape */

  duk_put_function_list(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);

  return 1;  /* return module value */
}