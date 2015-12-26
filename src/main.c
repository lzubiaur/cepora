#include <stdlib.h>
#include <stdio.h>

#include "duktape.h"
#include "macros.h"

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

/* function (id, require, exports, module) */
duk_ret_t require_handler(duk_context *ctx)
{
  const char *filename = duk_to_string(ctx, 0);
  DBG(ctx, "Read module %s", filename);
  char *dot = strrchr(filename, '.');
  if (dot && !strcmp(dot, ".coffee")) {
    DBG(ctx, "Compile CoffeeScript module %s", filename);
    duk_get_global_string(ctx, "CoffeeScript");
    duk_push_string(ctx, "compile");
    duk_push_string_file(ctx, filename);
    duk_pcall_prop(ctx, -3, 1);
  } else {
    duk_push_string_file(ctx, filename);
  }

  INF(ctx, duk_get_string(ctx, -1));

  return 1;
}

/* Log the error stack trace. Check if index `idx` is valid and the object
 * inherits from `error`.
 */
void dump_stack_trace(duk_context *ctx, duk_idx_t idx)
{
  if (duk_is_error(ctx, idx) && duk_get_prop_string(ctx, idx, "stack")) {
    ERR(ctx, duk_safe_to_string(ctx, -1));
  }
}

void fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg)
{
  FTL(ctx, "Fatal error: %s [code: %d]", msg, code);
  dump_stack_trace(ctx, -1);
  /* Fatal handler should not return. */
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  /* Create duktape VM heap */
  /* TODO investigate memory management implementations like tcmalloc
   * (https://github.com/gperftools/gperftools) and jmalloc
   * (https://github.com/jemalloc/jemalloc).
   */
  duk_context *ctx = NULL;
  ctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_handler);

  if (!ctx) {
    fprintf(stderr, "FATAL: Failed to create a Duktape heap.\n");
    exit(EXIT_FAILURE);
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

  /* Store command line arguments in the `Duktape` global object */
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "Duktape");
  duk_push_string(ctx, "argv");
  duk_idx_t arr_idx = duk_push_array(ctx); /* push an empty array */

  for (int i=0; i<argc; ++i) {
    duk_push_string(ctx, argv[i]);
    duk_put_prop_index(ctx, arr_idx, i);
  }

  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Non writable property */
  duk_pop_2(ctx);

  duk_get_global_string(ctx, "Duktape");
  duk_push_c_function(ctx, require_handler, 4);
  duk_put_prop_string(ctx, -2, "modSearch");
  duk_pop(ctx);

  if (duk_peval_file(ctx, "js/lib/coffee-script.js") != 0) {
    dump_stack_trace(ctx, -1);
    goto finished;
  }

  /* Run the script entry point. Filename is taken from command line or
  * will default to 'js/process.js'
  */
  const char *filename = argc > 1 ? argv[1] : "js/process.js";

  duk_get_global_string(ctx, "require");
  duk_push_string(ctx, filename);
  if (duk_pcall(ctx, 1) != 0) {
    dump_stack_trace(ctx, -1);
    goto finished;
  }
  // TODO how to return code from javasctipt ?
  // INF(ctx, "Script succeed with code: %s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);

finished:
  duk_destroy_heap(ctx);
  return EXIT_SUCCESS;
}
