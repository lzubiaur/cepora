/*
 * cpr_main.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include <stdio.h>
#include <stdlib.h> /* getenv */
#include <unistd.h> /* chdir */
#include <string.h> /* strcmp */
#include <dlfcn.h>

#include "duktape.h"
#include "cpr_macros.h"
#include "cpr_error.h"
#include "cpr_sys_tools.h"
#include "cpr_config.h"
#include "cpr_mod_coffee.h"

#define CPR_VERSION_STRING "v0.10.99"

void log_raw(const char*fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}

/* @javascript: reads a file from disk, and returns a string or `undefined`. */
duk_ret_t readfile(duk_context *ctx)
{
  /* It's not not mandatory to check the number of parameters passed to this
  * function using `duk_get_top` because it's guaranteed to be one (the stack will have one
  * and only one value (see how the function is binded using
  * `duk_push_c_function`).
  * Instead we must check if the parameter is either `null` or `undefined`.
  * There is only one parameter so we can check the top of the stack (-1).
  */
  if (duk_is_null_or_undefined(ctx, -1)) {
    duk_push_undefined(ctx);
  } else {
    duk_push_string_file(ctx, duk_to_string(ctx, 0));
  }

  return 1;
}

/* Helper function to set logging level for C API. This can be changed in
 * javascript by updating `Duktape.Logger.clog.l` to 'TRC', 'DBG', 'INF'...
 */
void set_C_log_level(duk_context *ctx, const char *level)
{
  duk_get_global_string(ctx, "Duktape");
  duk_get_prop_string(ctx, -1, "Logger");
  duk_get_prop_string(ctx, -1, "clog");
  duk_push_string(ctx, level);
  duk_put_prop_string(ctx, -2, "l");
}

void load_C_module(duk_context *ctx, const char *filename, const char *init_name)
{
  void *handle;
  duk_c_function init;
  char *error;

  handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
  if (!handle) {
    log_raw(dlerror());
    return;
  }

  /* Get the module's init function */
  init = (duk_c_function) dlsym(handle, init_name);
  if ((error = dlerror()) != NULL)  {
    log_raw(dlerror());
    goto finished;
  }

  /* Call the module's init function */
  duk_push_c_function(ctx, init, 0);
  duk_call(ctx, 0);

  /* The init function should return (push) an object with the exported
   * functions/properties. Those exported functions are then copied to the
   * `exports` table so they are available outside the C module.
   */
  duk_enum(ctx, -1, DUK_ENUM_INCLUDE_NONENUMERABLE | DUK_ENUM_OWN_PROPERTIES_ONLY | DUK_ENUM_INCLUDE_INTERNAL);
  while (duk_next(ctx, -1 /*enum_index*/, 1 /*get_value*/)) {
    duk_put_prop(ctx, 2); /* `exports` table is the third parameters (at idx 2 on the stack) */
  }
  duk_pop(ctx);  /* pop enum object */

finished:
  dlclose(handle);
}

/* @javascript
 * @params id, require, exports, module
 */
duk_ret_t require_handler(duk_context *ctx)
{
  const char *filename = duk_to_string(ctx, 0);
  /* TODO Lazy file extension check  */
  char *ext = strrchr(filename, '.');
  if (ext && strcmp(ext, ".coffee") == 0) {
    INF(ctx, "Load CoffeeScript module '%s'", filename);
    duk_get_global_string(ctx, "coffee");
    duk_push_string(ctx, "compile_coffee");
    duk_push_string(ctx, filename);
    if (duk_pcall_prop(ctx, -3, 1) != DUK_EXEC_SUCCESS) {
      // duk_error(ctx, DUK_ERR_RANGE_ERROR, "argument out of range: %d", (int) argval);
      ERR(ctx, "Cannot compile CoffeeScript '%s'", filename);
      dump_stack_trace(ctx, -1);
    }
  } else if (ext && strcmp(ext, ".dylib") == 0) {
    INF(ctx, "Load C module '%s'", filename);
    load_C_module(ctx, filename, "dukopen_io");
    duk_push_undefined(ctx); /* Return undefined because no source code. */
  } else {
    INF(ctx, "Load Javascript module '%s'", filename);
    duk_push_string_file(ctx, filename);
  }

  return 1;
}

/* Usage inspired from Node.js */
void usage()
{
  printf("Usage: cepora [options] [script.js | script.coffee] [arguments]\n");
  printf("\n");
  printf("Options:\n");
  printf("  -v, --version    print version\n");
  printf("  -h, --help       print this message\n");
  printf("\n");
  printf("Environment variables:\n");
  printf("CPR_PATH           directory prefixed to the module search path. If not set the \n");
  printf("                   executable path is used as default modules root directory.\n");
  printf("\n");
  exit(EXIT_SUCCESS);
}

void version()
{
  printf("Cepora %s - Git commit %s\n", CPR_VERSION_STRING, CPR_GIT_COMMIT);
  printf("Duktape %s\n", DUK_GIT_DESCRIBE);
  exit(EXIT_SUCCESS);
}

void fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg)
{
  FTL(ctx, "Fatal error: %s [code: %d]", msg, code);
  dump_stack_trace(ctx, -1);
  /* Fatal handler should not return. */
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  duk_context *ctx = NULL;
  char *path = NULL;
  const char *full_path = NULL, *filename = NULL;

  if (argc > 1) {
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0){
      version();
    } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      usage();
    }
  }

  /* Load script passed from command line or load './process.js' by default. */
  filename = argc > 1 ? argv[1] : "./process.js";

  /* Create duktape VM heap */
  /* TODO investigate memory management implementations like tcmalloc
   * (https://github.com/gperftools/gperftools) and jmalloc
   * (https://github.com/jemalloc/jemalloc).
   */
  ctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_handler);

  if (!ctx) {
    log_raw("FATAL: Failed to create a Duktape heap.\n");
    exit(EXIT_FAILURE);
  }

  /* TODO set log level from command line */
  set_C_log_level(ctx, "TRC");

  /* Look for CPR_PATH environment variable. If CPR_PATH is not defined, try to
  * retreive the process executable path.
  */
  if ((path = getenv("CPR_PATH")) != NULL) {
    DBG(ctx, "CPR_PATH defined: '%s'", path);
    path = strdup(path);
  } else if ((path = cpr_get_exec_dir()) == NULL) {
    FTL(ctx, "Cannot retrieve executable path. Please set 'CPR_PATH' to the install directory and try again.");
    goto finished;
  }

  /* Note regarding function binding parameters.
  * The third argument of `duk_push_c_function` is the number of parameters the
  * C function expects. It can be variable (DUK_VARARGS) or set to a
  * maximum number of parameters.
  * If a maximum of parameters is explicitly defined then the C functions will
  * get a value stack with all the expected parameters with the missing
  * parameters set to `undefined` and the extra parameter ignored.
  * If DUK_VARARGS is used then `duk_get_top` will return the number of
  * parameter passed.
  */
  duk_push_global_object(ctx);
  duk_push_c_function(ctx, readfile, 1); /* C function with exactly one argument */
  duk_put_prop_string(ctx, -2 /*index of global*/, "readfile");
  duk_pop(ctx);  /* pop global */

  /* Load module coffee */
  duk_push_global_object(ctx);
  duk_push_c_function(ctx, dukopen_coffee, 0);
  duk_call(ctx, 0);
  duk_put_global_string(ctx, "coffee");
  duk_pop(ctx); /*  pop global */

  /* Store command line arguments in the `Duktape` global object. */
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "Duktape");
  duk_push_string(ctx, "arguments");
  duk_idx_t arr_idx = duk_push_array(ctx); /* push an empty array */

  for (int i=2; i<argc; ++i) { /* Start at argument 2 (0: proccess, 1: script) */
    duk_push_string(ctx, argv[i]);
    duk_put_prop_index(ctx, arr_idx, i - 2);
  }

  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Non writable property */
  duk_pop_2(ctx);

  duk_get_global_string(ctx, "Duktape");
  duk_push_c_function(ctx, require_handler, 4);
  duk_put_prop_string(ctx, -2, "modSearch");
  duk_pop(ctx);

  /* Get CoffeeScript compiler full path */
  duk_push_string(ctx, path);
  duk_push_string(ctx, "/js/lib/coffee-script.js");
  duk_concat(ctx, 2);
  full_path = duk_get_string(ctx, -1);
  // DBG(ctx, "Load CoffeeScript compiler: %s", full_path);

  if (duk_peval_file(ctx, full_path) != 0) {
    ERR(ctx, "Error loading CoffeeScript compiler: '%s'", full_path);
    dump_stack_trace(ctx, -1);
    goto finished;
  }
  duk_pop(ctx); /* pop full_path */

  duk_get_global_string(ctx, "coffee");
  duk_push_string(ctx, "eval_script");
  duk_push_string(ctx, filename);
  if (duk_pcall_prop(ctx, -3, 1) != DUK_EXEC_SUCCESS) {
    /* If duk_safe_call fails the error object is at the top of the context.
     * But we must request at least one return value to actually get the error
     * object on the stack. */
    // ERR(ctx, "Error processing script '%s'", filename); /* Not revelant error message */
    dump_stack_trace(ctx, -1);
    goto finished;
  }
  // TODO how to return code from javasctipt ?
  // INF(ctx, "Script succeed with code: %s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);

  INF(ctx, "Bye!");

finished:
  duk_destroy_heap(ctx);
  return EXIT_SUCCESS;
}
