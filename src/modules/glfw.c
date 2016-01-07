/*
 * cpr_glfw.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "duktape.h"
#include "glfw.h"
#include "GLFW/glfw3.h"

static duk_ret_t glfw_init(duk_context *ctx) {
  int rc;
  rc = glfwInit();
  duk_push_boolean(ctx, rc);
  return 1;
}

static duk_ret_t glfw_terminate(duk_context *ctx) {
  glfwTerminate();
  return 0;
}

static const duk_function_list_entry module_funcs[] = {
    { "init", glfw_init, 0 },
    { "terminate", glfw_terminate, 0},
    { NULL, NULL, 0 }
};

duk_ret_t dukopen_glfw(duk_context *ctx) {
  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);

  return 1;  /* return module value */
}
