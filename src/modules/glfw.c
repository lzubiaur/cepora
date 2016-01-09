/*
 * cpr_glfw.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "duktape.h"
#include "glfw.h"
#include "GLFW/glfw3.h"

#include "cpr_macros.h"

#define GLFW_ERR_CALLBACK_STASH_KEY "glfwErrCallbackKey"
#define GLFW_KEY_CALLBACK_STASH_KEY "glfwKeyCallbackKey"

/* global reference to the duktape context. Required for GLFW callbacks */
/* XXX is there any workaround to avoid keeping a global reference to the context? */
/* TODO use duk_get_heapptr + duk_push_heapptr for performance */
static duk_context *_ctx = NULL;

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

void error_callback(int error, const char* description)
{
  duk_push_global_stash(_ctx);
  duk_get_prop_string(_ctx, -1, GLFW_ERR_CALLBACK_STASH_KEY);
  duk_push_int(_ctx, error);
  duk_push_string(_ctx, description);
  duk_call(_ctx, 2);
}

static duk_ret_t glfw_set_error_callback(duk_context *ctx) {
  if (duk_is_function(ctx, 0) == 0) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "not a function");
  }
  duk_push_global_stash(ctx);
  duk_dup(ctx, 0); /* Push the error function callback  */
  duk_put_prop_string(ctx, -2, GLFW_ERR_CALLBACK_STASH_KEY);

  glfwSetErrorCallback(error_callback);
  return 0;
}

static duk_ret_t glfw_window_should_close(duk_context *ctx) {
  int rc = 0;
  rc = glfwWindowShouldClose(duk_require_pointer(ctx ,0));
  duk_push_boolean(ctx, rc);
  return 1;
}

static duk_ret_t glfw_destroy_window(duk_context *ctx) {
  glfwDestroyWindow(duk_require_pointer(ctx, 0));
  return 0;
}

static duk_ret_t glfw_swap_buffers(duk_context *ctx) {
  glfwSwapBuffers(duk_require_pointer(ctx, 0));
  return 0;
}

static duk_ret_t glfw_pool_events(duk_context *ctx) {
  glfwPollEvents();
  return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  duk_push_global_stash(_ctx);
  duk_get_prop_string(_ctx, -1, GLFW_KEY_CALLBACK_STASH_KEY);
  duk_push_pointer(_ctx, window);
  duk_push_int(_ctx, key);
  duk_push_int(_ctx, scancode);
  duk_push_int(_ctx, action);
  duk_push_int(_ctx, mods);
  duk_call(_ctx, 5);
}

static duk_ret_t glfw_set_key_callback(duk_context *ctx) {
  if (duk_is_function(ctx, 1) == 0) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "not a function");
  }
  duk_push_global_stash(ctx);
  duk_dup(ctx, 1); /* Push the callback function */
  duk_put_prop_string(ctx, -2, GLFW_KEY_CALLBACK_STASH_KEY);

  glfwSetKeyCallback(duk_require_pointer(ctx, 0), key_callback);
  return 0;
}

static duk_ret_t glfw_set_window_should_close(duk_context *ctx) {
  glfwSetWindowShouldClose(duk_require_pointer(ctx, 0), duk_require_boolean(ctx, 1));
  return 0;
}

static const duk_function_list_entry module_funcs[] = {
  { "init",                 glfw_init,                    0 },
  { "terminate",            glfw_terminate,               0 },
  { "createWindow",         glfw_create_window,           3 },
  { "makeContextCurrent",   glfw_make_context_current,    1 },
  { "setErrorCallBack",     glfw_set_error_callback,      1 },
  { "windowShouldClose",    glfw_window_should_close,     1 },
  { "destroyWindow",        glfw_destroy_window,          1 },
  { "swapBuffers",          glfw_swap_buffers,            1 },
  { "poolEvents",           glfw_pool_events,             0 },
  { "setKeyCallback",       glfw_set_key_callback,        2 },
  { "setWindowShouldClose", glfw_set_window_should_close, 2 },
  { NULL, NULL, 0 }
};

const duk_number_list_entry module_consts[] = {
  { "GLFW_RELEASE",       (double)0   },
  { "GLFW_PRESS",         (double)1   },
  { "GLFW_REPEAT",        (double)2   },
  { "GLFW_KEY_UNKNOWN",   (double)-1  },
  { "GLFW_KEY_ESCAPE",    (double)256 },
  { NULL, 0.0 }
};

duk_ret_t dukopen_glfw(duk_context *ctx) {
  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);

  _ctx = ctx;

  return 1;  /* return module value */
}
