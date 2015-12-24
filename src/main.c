#include <stdlib.h>
#include <stdio.h>

#include "duktape.h"

duk_ret_t readfile(duk_context *ctx)
{
  FILE *fp = NULL;
  char *str = NULL;
  long fsize = 0;

  /* It's not not mandatory to check the number of parameters passed to this
  * function because it's guaranteed to be one (the stack will have one
  * and only one value (see how the function is binded using
  * `duk_push_c_function`)/
  /*
  if (duk_get_top(ctx) == 0) {
    return DUK_RET_TYPE_ERROR;
  }
  */

  /* Throw `TypeError` if no arguments is given.
  * Check if the parameter is either `null` or `undefined`. There is only one
  * parameter so we can check the top of the stack (-1) */
  if (duk_is_null_or_undefined(ctx, -1)) {
    return DUK_RET_TYPE_ERROR;
  }

  const char *filename = duk_to_string(ctx, 0);

  if((fp = fopen(filename, "r")) == NULL) {
    fclose(fp);
    duk_error(ctx, 1, "Cannot find module %s", filename);
  }
  fseek(fp, 0, SEEK_END);
  fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  str = malloc(fsize + 1);
  fread(str, fsize, 1, fp);
  fclose(fp);

  str[fsize] = 0;

  fclose(fp);
  duk_push_lstring(ctx, str, fsize);
  return 1;  /* return one value */
}

int main(int argc, char *argv[]) {
  // TODO use duk_create_heap() instead
  duk_context *ctx = duk_create_heap_default();

  if (!ctx) {
    printf("Failed to create a Duktape heap.\n");
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

  /* Run the script entry point */

  if (duk_peval_file(ctx, "js/process.js") != 0) {
    printf("Error: %s\n", duk_safe_to_string(ctx, -1));
    goto finished;
  }
  duk_pop(ctx);

finished:
  duk_destroy_heap(ctx);
  return EXIT_SUCCESS;
}
