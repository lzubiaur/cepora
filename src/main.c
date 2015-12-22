#include <stdlib.h>
#include <stdio.h>

#include "duktape.h"

int readfile(duk_context *ctx)
{
  FILE *fp = NULL;
  char *str = NULL;
  long fsize = 0;
  int n = duk_get_top(ctx);  /* #args passed to the function */

  // TODO check parameters

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
  return 1;  /* one return value */
}

int main(int argc, char *argv[]) {
  duk_context *ctx = duk_create_heap_default();

  if (!ctx) {
    printf("Failed to create a Duktape heap.\n");
    exit(EXIT_FAILURE);
  }

  duk_push_global_object(ctx);
  duk_push_c_function(ctx, readfile, DUK_VARARGS);
  duk_put_prop_string(ctx, -2 /*idx:global*/, "readfile");
  duk_pop(ctx);  /* pop global */

  if (duk_peval_file(ctx, "js/process.js") != 0) {
    printf("Error: %s\n", duk_safe_to_string(ctx, -1));
    goto finished;
  }
  duk_pop(ctx);

finished:
  duk_destroy_heap(ctx);
  return EXIT_SUCCESS;
}
