#include "cpr_io.h"
#include "cpr_macros.h"

/* Load and run JavaScript and CoffeeScript file.  */
static duk_ret_t read(duk_context *ctx)
{
  printf("Hello module\n");
  return 0;
}

static const duk_function_list_entry module_funcs[] = {
    { "read", read, 1 },
    { NULL, NULL, 0 }
};

const duk_number_list_entry module_consts[] = {
    { "FLAG_FOO", (double) (1 << 0) },
    { "FLAG_BAR", (double) (1 << 1) },
    { "FLAG_QUUX", (double) (1 << 2) },
    { "DELAY", 300.0 },
    { NULL, 0.0 }
};

duk_ret_t dukopen_io(duk_context *ctx)
{
  DBG(ctx, "dukopen_io");
  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);

  return 1;  /* return module value */
}
