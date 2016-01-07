/*
 * cpr_glfw.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "duktape.h"
#include "glfw.h"

duk_ret_t glfw_init(duk_context *ctx) {
  glfwInit();
}

static const duk_function_list_entry module_funcs[] = {
    { "init", glfw_init, 0 },
    { NULL, NULL, 0 }
};

duk_ret_t dukopen_io(duk_context *ctx) {
  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);

  return 1;  /* return module value */
}
