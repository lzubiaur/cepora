#include "cpr_dummy.h"
#include "cpr_macros.h"

CPR_API_INTERN duk_ret_t foo(duk_context *ctx)
{
  printf("%s", duk_get_string(ctx, 0));
  return 0;
}

CPR_API_INTERN const duk_function_list_entry module_funcs[] = {
  { "foo", foo, 1 },
  { NULL, NULL, 0 }
};

/* Linux: if `static` is omitted the constant are `undefined` */
CPR_API_INTERN const duk_number_list_entry module_consts[] = {
  { "BAR", (double)(-1) },
  { NULL, 0.0 }
};


CPR_API_EXTERN duk_ret_t dukopen_dummy(duk_context *ctx) {
  DBG(ctx, "dukopen_dummy");
  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);

  return 1;  /* return module value */
}
