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

static duk_ret_t glfw_create_window(duk_context *ctx) {
  GLFWwindow* window = NULL;
  int width = 0;
  int height = 0;
  const char *title = NULL;

  width = duk_require_int(ctx, 0);
  height = duk_require_int(ctx, 1);
  title = duk_require_string(ctx, 2);

  window = glfwCreateWindow(width, height, title, NULL, NULL);
  duk_push_pointer(ctx, window);
  return 1;
}

static duk_ret_t glfw_make_context_current(duk_context *ctx)
{
  GLFWwindow *window = NULL;
  window = duk_require_pointer(ctx, 0);
  glfwMakeContextCurrent(window);
  return 0;
}

static duk_context *error_ctx = NULL;
static void *error_callback_ptr = NULL;

void error_callback(int error, const char* description)
{
  duk_push_heapptr(error_ctx, error_callback_ptr);
  duk_push_int(error_ctx, error);
  duk_push_string(error_ctx, description);
  duk_call(error_ctx, 2);
}

static duk_ret_t glfw_set_error_callback(duk_context *ctx) {
  // duk_is_ecmascript_function(ctx, 0);
  error_ctx = ctx;
  error_callback_ptr = duk_get_heapptr(ctx, 0);
  glfwSetErrorCallback(error_callback);
  return 0;
}

static duk_ret_t glfw_window_should_close(duk_context *ctx) {
  int rc = 0;
  void * window = NULL;
  window = duk_require_pointer(ctx, 0);
  rc = glfwWindowShouldClose(window);
  duk_push_boolean(ctx, rc);
  return 0;
}

static const duk_function_list_entry module_funcs[] = {
    { "init",               glfw_init,                  0 },
    { "terminate",          glfw_terminate,             0 },
    { "createWindow",       glfw_create_window,         3 },
    { "makeContextCurrent", glfw_make_context_current,  1 },
    { "setErrorCallBack",   glfw_set_error_callback,    1 },
    { "windowShouldClose",  glfw_window_should_close,   1 },
    { NULL, NULL, 0 }
};

duk_ret_t dukopen_glfw(duk_context *ctx) {
  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);

  return 1;  /* return module value */
}
