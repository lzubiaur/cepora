/*
 * cpr_gl.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_gl.h"
#include "GL/gl3w.h"

static duk_ret_t cpr_gl_clear(duk_context *ctx) {
  glClear(duk_require_int(ctx, 0));
  return 0;
}

static duk_ret_t cpr_gl_clear_depth(duk_context *ctx) {
  glClearDepth(duk_require_int(ctx, 0));
  return 0;
}

static duk_ret_t cpr_gl_enable(duk_context *ctx) {
  glEnable(duk_require_int(ctx, 0));
  return 0;
}

static duk_ret_t cpr_gl_clear_color(duk_context *ctx) {
  glClearColor(duk_require_number(ctx, 0),
               duk_require_number(ctx, 1),
               duk_require_number(ctx, 2),
               duk_require_number(ctx, 3));
  return 0;
}

static duk_ret_t cpr_gl_viewport(duk_context *ctx) {
  glViewport(duk_require_int(ctx, 0),
             duk_require_int(ctx, 1),
             duk_require_int(ctx, 2),
             duk_require_int(ctx, 3));
  return 0;
}

duk_ret_t dukopen_gl(duk_context *ctx) {
  const duk_function_list_entry module_funcs[] = {
    { "clear", cpr_gl_clear, 1 },
    { "enable", cpr_gl_enable, 1 },
    { "clearDepth", cpr_gl_clear_depth, 1 },
    { "clearColor", cpr_gl_clear_color, 4},
    { "viewport", cpr_gl_viewport, 4},
    { NULL, NULL, 0 }
  };

  const duk_number_list_entry module_consts[] = {
    { "DEPTH_TEST", (double)(GL_DEPTH_TEST) },
    { "COLOR_BUFFER_BIT", (double)(GL_COLOR_BUFFER_BIT) },
    { "DEPTH_BUFFER_BIT", (double)(GL_DEPTH_BUFFER_BIT) },
    { NULL, 0.0 }
  };

  /* duk_push_object(ctx); */  /* object is passed by the caller */
  duk_put_function_list(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);

  return 1; /* return the object passed as paramter */
}
